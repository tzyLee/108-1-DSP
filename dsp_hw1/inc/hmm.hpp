#ifndef HMM_HPP_
#define HMM_HPP_

#include "hmm.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <vector>

struct HiddenMarkovModel : HMM {
  HiddenMarkovModel(int obs_len) : obs_len{obs_len} {}
  ~HiddenMarkovModel() {
    if (model_name)
      free(model_name);
  }

  const int obs_len;
  int seq_num;
  std::vector<char> sequences;
  std::vector<double> accGamma, accEpsilon, accGammaObs, accGamma0;
  mutable std::vector<double> alpha, beta, gamma, epsilon, delta;

  void baumWelchAlgorithm(const int iterations) {
    alpha.resize(obs_len * state_num);
    beta.resize(obs_len * state_num);
    gamma.resize(obs_len * state_num);
    epsilon.resize((obs_len - 1) * state_num * state_num);
    accGamma.resize(obs_len * state_num);
    accEpsilon.resize(state_num * state_num);
    accGammaObs.resize(state_num * observ_num);
    accGamma0.resize(state_num);

    for (int i = 0; i < iterations; ++i) {
      for (int j = 0; j < seq_num; ++j) {
        // Reset gamma and epsilon
        std::fill(gamma.begin(), gamma.end(), 0);
        std::fill(epsilon.begin(), epsilon.end(), 0);
        // Compute alpha and beta
        forward(&sequences[j * (obs_len + 1)]);
        backward(&sequences[j * (obs_len + 1)]);
        // Compute gamma and epsilon
        updateGamma();
        updateEpsilon(&sequences[j * (obs_len + 1)]);
        // Accumulate gamma and epsilon
        for (int t = 0; t < obs_len; ++t)
          for (int i = 0; i < state_num; ++i)
            accGamma[ind(t, i, state_num)] += gamma[ind(t, i, state_num)];
        for (int i = 0; i < state_num; ++i)
          accGamma0[i] += gamma[i];
        for (int k = 0; k < state_num; ++k)
          for (int t = 0; t < obs_len; ++t)
            accGammaObs[ind(k, sequences[ind(j, t, (obs_len + 1))] - 'A',
                            observ_num)] += gamma[ind(t, k, state_num)];
        for (int t = 0; t < obs_len - 1; ++t)
          for (int i = 0; i < state_num; ++i)
            for (int j = 0; j < state_num; ++j)
              accEpsilon[ind(i, j, state_num)] +=
                  epsilon[ind(ind(t, i, state_num), j, state_num)];
      }
      updateParam();
      std::fill(accGamma.begin(), accGamma.end(), 0);
      std::fill(accEpsilon.begin(), accEpsilon.end(), 0);
      std::fill(accGammaObs.begin(), accGammaObs.end(), 0);
      std::fill(accGamma0.begin(), accGamma0.end(), 0);
    }
  }

  void forward(const char *obs) const {
    // Initialization: alpha_0(state) = pi_state * b_state(observation_0)
    for (int i = 0; i < state_num; ++i)
      alpha[i] = initial[i] * observation[obs[0] - 'A'][i];
    // Induction: alpha_t(cur) = [ \sum_{pre} alpha_{t-1}(pre) * a_{pre}{cur} ]
    // * b_{pre}(O_t)
    for (int t = 1; t < obs_len; ++t)
      for (int cur = 0; cur < state_num; ++cur) {
        double sum = 0;
        for (int pre = 0; pre < state_num; ++pre)
          sum += transition[pre][cur] * alpha[ind(t - 1, pre, state_num)];
        alpha[ind(t, cur, state_num)] = sum * observation[obs[t] - 'A'][cur];
      }
  }

  void backward(const char *obs) const {
    // Initialization: beta_T(state) = 1;
    std::fill_n(beta.rbegin(), state_num, 1);
    // Induction: beta_t(cur) = sum_{next} a_{cur}{next} * b_{next}(O_{t+1}) *
    // beta_{t+1}(next)
    for (int t = obs_len - 2; t >= 0; --t)
      for (int cur = 0; cur < state_num; ++cur) {
        double sum = 0;
        for (int next = 0; next < state_num; ++next)
          sum += transition[cur][next] * observation[obs[t + 1] - 'A'][next] *
                 beta[ind(t + 1, next, state_num)];
        beta[ind(t, cur, state_num)] = sum;
      }
  }

  void prepareDelta() const { delta.resize(obs_len * state_num); }

  double viterbiAlgorithm(const char *obs) const {
    // assume delta.size() is (obs_len * state_num);
    // Initialization: delta_0(i) = pi_i * b_i(O_0)
    for (int i = 0; i < state_num; ++i)
      delta[i] = initial[i] * observation[obs[0] - 'A'][i];
    // Reduction
    for (int t = 0; t < obs_len - 1; ++t)
      for (int j = 0; j < state_num; ++j) {
        double max = 0;
        for (int i = 0; i < state_num; ++i)
          max = std::max(max, delta[ind(t, i, state_num)] * transition[i][j]);
        delta[ind(t + 1, j, state_num)] =
            max * observation[obs[t + 1] - 'A'][j];
      }

    // Termination
    return *std::max_element(delta.begin() + (obs_len - 1) * state_num,
                             delta.end());
  }

  void updateGamma() const {
    for (int t = 0; t < obs_len; ++t) {
      double p = 0;
      // P(O | lambda) = \sum_{i} alpha_t(i) * beta_t(i)
      // gamma_t(i) = alpha_t(i) * beta_t(i) / P(O | lambda)
      for (int i = 0; i < state_num; ++i) {
        double temp = alpha[ind(t, i, state_num)] * beta[ind(t, i, state_num)];
        gamma[ind(t, i, state_num)] += temp;
        p += temp;
      }
      for (int i = 0; i < state_num; ++i)
        gamma[ind(t, i, state_num)] /= p;
    }
  }

  void updateEpsilon(const char *obs) const {
    for (int t = 0; t < obs_len - 1; ++t) {
      double p = 0;
      // \sum_i \sum_j [ alpha_t(i) * a_{ij} * b_j(O_{t+1}) * beta_{t+1}(j)]
      // epsilon_t(i, j) = alpha_t(i) * a_{ij} * b_j(O_{t+1}) * beta_{t+1}(j) /
      // P(O | lambda)
      for (int i = 0; i < state_num; ++i)
        for (int j = 0; j < state_num; ++j) {
          double temp = alpha[ind(t, i, state_num)] * transition[i][j] *
                        observation[obs[t + 1] - 'A'][j] *
                        beta[ind(t + 1, j, state_num)];
          epsilon[ind(ind(t, i, state_num), j, state_num)] += temp;
          p += temp;
        }
      for (int i = 0; i < state_num; ++i)
        for (int j = 0; j < state_num; ++j)
          epsilon[ind(ind(t, i, state_num), j, state_num)] /= p;
    }
  }

  void updateParam() {
    // update pi
    for (int i = 0; i < state_num; ++i)
      initial[i] = accGamma0[i] / seq_num;

    // update a_{ij}
    for (int i = 0; i < state_num; ++i) {
      double gammaSum = 0;
      for (int t = 0; t < obs_len - 1; ++t)
        gammaSum += accGamma[ind(t, i, state_num)];
      for (int j = 0; j < state_num; ++j) {
        transition[i][j] = accEpsilon[ind(i, j, state_num)] / gammaSum;
      }
    }

    // update b_j(k)
    for (int j = 0; j < state_num; ++j) {
      double gammaSum = 0;
      for (int t = 0; t < obs_len; ++t)
        gammaSum += accGamma[ind(t, j, state_num)];
      for (int k = 0; k < observ_num; ++k)
        observation[k][j] = accGammaObs[ind(j, k, observ_num)] / gammaSum;
    }
  }

  void readObservations(const char *seq_path) {
    std::ifstream ifs(seq_path, std::ifstream::in | std::ifstream::binary);
    if (!ifs) {
      perror(seq_path);
      exit(1);
      return;
    }
    ifs.seekg(0, std::ifstream::end);
    std::streampos size = ifs.tellg();
    sequences.resize(size);
    ifs.seekg(0, std::ifstream::beg);
    ifs.read(sequences.data(), size);
    seq_num = size / (obs_len + 1); // +1 for \n
  }

  void loadParam(const char *filename) { loadHMM(this, filename); }

  void dumpParam(const char *filename) {
    FILE *outFile = open_or_die(filename, "w");
    dumpHMM(outFile, this);
    fclose(outFile);
  }

  void release() {
    free(model_name);
    model_name = nullptr;
  }

  static constexpr int ind(const int i, const int j, const int width) {
    return i * width + j;
  }
};

#endif