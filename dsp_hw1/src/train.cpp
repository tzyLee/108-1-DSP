#include <cstdlib>
#include <iostream>
#include <vector>
#include <hmm.h>
#include "hmm.hpp"

constexpr int ind(int i, int j, int w)
{
    return i * w + j;
}

/* 
    alpha[T][STATE]
    alpha_T1(state_0) alpha_T1(state_1)
    alpha_T2(state_1) alpha_T2(state_1)
    alpha_T3(state_2) alpha_T3(state_1)
*/
void forward(const HMM &model, const char *obs, const int obs_len, std::vector<double> &alpha)
{
    for (int state = 0; state < model.state_num; ++state)
    {
        alpha[state] = model.initial[state] * model.observation[obs[0] - 'A'][state];
    }
    for (int t = 1; t < obs_len; ++t)
    {
        for (int curState = 0; curState < model.state_num; ++curState)
        {
            double sum = 0;
            for (int prevState = 0; prevState < model.state_num; ++prevState)
            {
                sum += model.transition[prevState][curState] * alpha[ind(t - 1, prevState, model.state_num)];
            }
            alpha[ind(t, curState, model.state_num)] = sum * model.observation[obs[t] - 'A'][curState];
        }
    }
}

void backward(const HMM &model, const char *obs, const int obs_len, std::vector<double> &beta)
{
    for (int state = 0; state < model.state_num; ++state)
    {
        beta[ind(obs_len - 1, state, model.state_num)] = 1;
    }
    for (int t = obs_len - 2; t >= 0; --t)
    {
        for (int state = 0; state < model.state_num; ++state)
        {
            double sum = 0;
            for (int nextState = 0; nextState < model.state_num; ++nextState)
            {
                sum += model.transition[state][nextState] * model.observation[obs[t + 1]][nextState] * beta[ind(t + 1, nextState, model.state_num)];
            }
            beta[ind(t, state, model.state_num)] = sum;
        }
    }
}

void calculateGamma(const HMM &model, const std::vector<double> &alpha, const std::vector<double> &beta, const int obs_len, std::vector<double> &gamma)
{
    for (int t = 0; t < obs_len; ++t)
    {
        double sum = 0; // TODO is P(O | lambda) the same?
        for (int state = 0; state < model.state_num; ++state)
        {
            sum += alpha[ind(t, state, model.state_num)] * beta[ind(t, state, model.state_num)];
        }
        for (int state = 0; state < model.state_num; ++state)
        {
            gamma[ind(t, state, model.state_num)] = alpha[ind(t, state, model.state_num)] * beta[ind(t, state, model.state_num)] / sum;
        }
    }
}

void calculateEpsilon(const HMM &model, std::vector<double> &alpha, std::vector<double> &beta, const char *obs, const int obs_len, std::vector<double> &epsilon)
{
    for (int t = 0; t < obs_len - 1; ++t)
    {
        double p = 0; // TODO is P(O | lambda) the same?
        for (int i = 0; i < model.state_num; ++i)
        {
            for (int j = 0; j < model.state_num; ++j)
            {
                p += alpha[ind(t, i, model.state_num)] * model.transition[i][j] * model.observation[obs[t + 1] - 'A'][j] * beta[ind(t + 1, j, model.state_num)];
            }
        }

        for (int i = 0; i < model.state_num; ++i)
        {
            for (int j = 0; j < model.state_num; ++j)
            {
                epsilon[ind(t, ind(i, j, model.state_num), model.state_num)] = alpha[ind(t, i, model.state_num)] * model.transition[i][j] * model.observation[obs[t + 1] - 'A'][j] * beta[ind(t + 1, j, model.state_num)] / p;
            }
        }
    }
}

void updateModel(HMM &model, std::vector<double> &gamma, std::vector<double> &epsilon, const int obs_len)
{
    for (int i = 0; i < model.state_num; ++i)
    {
        model.initial[i] = gamma[i]; // TODO divide by N
    }

    for (int i = 0; i < model.state_num; ++i)
    {
        double gam = 0;
        for (int t = 0; t < obs_len; ++t)
        {
            gam += gamma[ind(t, i, model.state_num)];
        }
        double eps = 0;
        for (int j = 0; j < model.state_num; ++j)
        {
            for (int t = 0; t < obs_len - 1; ++t)
            {
                eps += epsilon[ind(t, ind(i, j, model.state_num), model.state_num)];
            }
        }
    }
}

void baumWelchAlgorithm(const HMM &model, const char *observations, const int obs_len)
{
    std::vector<double> alpha(model.state_num * obs_len); // N*T
    std::vector<double> beta(model.state_num * obs_len);  // N*T
    std::vector<double> gamma(model.state_num * obs_len);
    std::vector<double> epsilon(model.state_num * model.state_num * (obs_len - 1));
    forward(model, observations, obs_len, alpha);
    backward(model, observations, obs_len, beta);
    calculateGamma(model, alpha, beta, obs_len, gamma);
    calculateEpsilon(model, alpha, beta, observations, obs_len, epsilon);
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cerr << "Not enough arguments\n";
        return -1;
    }
    int iteration = std::atoi(argv[1]);
    const char *model_init_path = argv[2];
    const char *seq_path = argv[3];
    const char *output_model_path = argv[4];

    HMM model;

    loadHMM(&model, model_init_path);

    for (int i = 0; i < iteration; ++i)
    {
        // TODO
    }

    FILE *outfile = open_or_die(output_model_path, "w");
    dumpHMM(outfile, &model);
    fclose(outfile);
    return 0;
}