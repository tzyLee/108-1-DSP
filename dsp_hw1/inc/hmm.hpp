#ifndef HMM_HPP_
#define HMM_HPP_

#include "hmm.h"
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <functional>
#include <numeric>

struct HiddenMarkovModel : HMM
{
    HiddenMarkovModel(int obs_len) : obs_len{obs_len} {}
    ~HiddenMarkovModel()
    {
        if (model_name)
            free(model_name);
    }

    const int obs_len;
    std::vector<double> accGamma, accEpsilon;
    mutable std::vector<double> alpha, beta, gamma, epsilon;

    void baumWelchAlgorithm(const int iterations, const int seq_num, const char *seq_path)
    {
        alpha.resize(obs_len * state_num);
        beta.resize(obs_len * state_num);
        gamma.resize(obs_len * state_num);
        epsilon.resize((obs_len - 1) * state_num * state_num);
        accGamma.resize(obs_len * state_num);
        accEpsilon.resize((obs_len - 1) * state_num * state_num);

        char sequences[seq_num][obs_len + 1];
        FILE *seqFile = open_or_die(seq_path, "r");
        fread(sequences, 1, sizeof(sequences), seqFile);
        fclose(seqFile);

        for (int i = 0; i < iterations; ++i)
        {
            std::fill(accGamma.begin(), accGamma.end(), 0);
            std::fill(accEpsilon.begin(), accEpsilon.end(), 0);
            for (int j = 0; j < seq_num; ++j)
            {
                // Reset gamma and epsilon
                std::fill(gamma.begin(), gamma.end(), 0);
                std::fill(epsilon.begin(), epsilon.end(), 0);
                // Compute alpha and beta
                forward(sequences[j]);
                backward(sequences[j]);
                // Compute gamma and epsilon
                updateGamma();
                updateEpsilon(sequences[j]);
                // Accumulate gamma and epsilon
                std::transform(gamma.begin(), gamma.end(), accGamma.begin(), accGamma.begin(), std::plus<double>());
                std::transform(epsilon.begin(), epsilon.end(), accEpsilon.begin(), accEpsilon.begin(), std::plus<double>());
            }
            updateParam(seq_num);
        }
    }

    void forward(const char *obs) const
    {
        // Initialization: alpha_0(state) = pi_state * b_state(observation_0)
        std::transform(initial, initial + state_num, observation[obs[0] - 'A'], alpha.begin(), std::multiplies<double>());
        // Induction: alpha_t(cur) = [ \sum_{pre} alpha_{t-1}(pre) * a_{pre}{cur} ] * b_{pre}(O_t)
        for (int t = 1; t < obs_len; ++t)
            for (int cur = 0; cur < state_num; ++cur)
            {
                double sum = 0;
                for (int pre = 0; pre < state_num; ++pre)
                    sum += transition[pre][cur] * alpha[ind(t - 1, pre, state_num)];
                alpha[ind(t, cur, state_num)] = sum * observation[obs[t] - 'A'][cur];
            }
    }

    void backward(const char *obs) const
    {
        // Initialization: beta_T(state) = 1;
        std::fill_n(beta.rbegin(), state_num, 1);
        // Induction: beta_t(cur) = sum_{next} a_{cur}{next} * b_{next}(O_{t+1}) * beta_{t+1}(next)
        for (int t = obs_len - 1; t >= 0; --t)
            for (int cur = 0; cur < state_num; ++cur)
            {
                double sum = 0;
                for (int next = 0; next < state_num; ++next)
                    sum += transition[cur][next] * observation[obs[t + 1] - 'A'][next] * beta[ind(t + 1, next, state_num)];
                beta[ind(t, cur, state_num)] = sum;
            }
    }

    void updateGamma() const
    {
        for (int t = 0; t < obs_len; ++t)
        {
            double p = 0;
            // P(O | lambda) = \sum_{i} alpha_t(i) * beta_t(i)
            // gamma_t(i) = alpha_t(i) * beta_t(i) / P(O | lambda)
            for (int i = 0; i < state_num; ++i)
            {
                double temp = alpha[ind(t, i, state_num)] * beta[ind(t, i, state_num)];
                gamma[ind(t, i, state_num)] += temp;
                p += temp;
            }
            for (int i = 0; i < state_num; ++i)
                gamma[ind(t, i, state_num)] /= p;
        }
    }

    void updateEpsilon(const char *obs) const
    {
        for (int t = 0; t < obs_len - 1; ++t)
        {
            double p = 0;
            // \sum_i \sum_j [ alpha_t(i) * a_{ij} * b_j(O_{t+1}) * beta_{t+1}(j)]
            // epsilon_t(i, j) = alpha_t(i) * a_{ij} * b_j(O_{t+1}) * beta_{t+1}(j) / P(O | lambda)
            for (int i = 0; i < state_num; ++i)
                for (int j = 0; j < state_num; ++j)
                {
                    double temp = alpha[ind(t, i, state_num)] * transition[i][j] * observation[obs[t + 1] - 'A'][j] * beta[ind(t + 1, j, state_num)];
                    epsilon[ind(t, ind(i, j, state_num), state_num)] += temp;
                    p += temp;
                }
            for (int i = 0; i < state_num; ++i)
                for (int j = 0; j < state_num; ++j)
                    epsilon[ind(t, ind(i, j, state_num), state_num)] /= p;
        }
    }

    void updateParam(const int seq_num)
    {
        // update pi
        for (int i = 0; i < state_num; ++i)
            initial[i] = accGamma[i] / seq_num;

        // update a_{ij}
        for (int i = 0; i < state_num; ++i)
        {
            double gammaSum = 0;
            for (int t = 0; t < obs_len - 1; ++t)
                gammaSum += accGamma[ind(t, i, state_num)];

            for (int j = 0; j < state_num; ++j)
            {
                double epsilonSum = 0;
                for (int t = 0; t < obs_len - 1; ++t)
                    epsilonSum += accEpsilon[ind(t, ind(i, j, state_num), state_num)];
                transition[i][j] = epsilonSum / gammaSum;
            }
        }

        // update b_j(k)
        for (int j = 0; j < state_num; ++j)
        {
            double gammaSum = 0;
            for (int t = 0; t < obs_len; ++t)
                gammaSum += accGamma[ind(t, j, state_num)];

            for (int k = 0; k < obs_len; ++k)
            {
                // TODO observation[k][j] = /* TODO */ / gammaSum;
            }
        }
    }

    void loadParam(const char *filename)
    {
        loadHMM(this, filename);
    }

    void dumpParam(const char *filename)
    {
        FILE *outFile = open_or_die(filename, "w");
        dumpHMM(outFile, this);
        fclose(outFile);
    }

    void release()
    {
        free(model_name);
        model_name = nullptr;
    }

    static constexpr int ind(const int i, const int j, const int width)
    {
        return i * width + j;
    }
};

#endif