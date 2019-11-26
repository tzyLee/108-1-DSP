1. 執行前兩個的時候都是 00 hours 00 mins 00 secs
2. 執行第三個的時候變成 No such file or directory

後來發現是在 script 裡面用的是相對於 /opt/kaldi 的路徑，把 dsp-hw2-1 放到 /opt/kaldi 就好了

![image-20191113163018086](/home/pythagordes/.config/Typora/typora-user-images/image-20191113163018086.png)

incgauss 是 `$[($totgauss - $numgauss)/$maxiterinc]` ，如果算出來小於 1，就不會增加

調整 acoustic weight

0.87 -> 75.07%

0.8 -> 76.11%

0.5 -> 81.35%

0.3 -> 85.84%

0.25 -> 87.10%

0.225 -> 87.91%

0.21 -> 87.97%

0.2 -> 88.14%

0.15 -> 88.49%

0.125 -> 88.77%

0.1245 -> 88.71%

0.12 -> 88.60%

0.1 -> 87.28%

0.05 -> 76.51%

選擇 0.125

調整 transition-scale

1.0 -> 87.8%

1.5 -> 88.14%

2.0 -> 88.77%

2.5 -> 88.77%

3.0 -> 88.6%

4.0 -> 88.54%

選擇 2.0

調整 acoustic-scale

0.1 -> 88.77%

0.5 -> 87.8%

1.0 -> 87.85%

1.5 -> 87.97%

2.0 -> 87.80%

選擇 1.5

調整 self loop scale

0.05 -> 87.91%

0.1 -> 87.97%

0.2 -> 87.97%

0.5-> 87.97%

1.0 -> 88.08%

1.1 -> 87.97

1.2 -> 88.14%

1.25 -> 88.08%

1.3 -> 88.03%

1.5 -> 77.89%

調整 numiters

```
numiters=15
maxiterinc=9
numgauss=1
totgauss=5
realign_iters="1 2 3 4 5 6 7 9 11 13";
88.2%
```

```
numiters=20
maxiterinc=9
numgauss=1
totgauss=5
realign_iters="1 2 3 4 5 6 7 9 11 13 15 17";
88.31%
```

```
numiters=20
maxiterinc=12
numgauss=4
totgauss=16
realign_iters="1 2 3 4 5 6 7 9 11 13 15 17";
88.31%
```

```
numiters=30
maxiterinc=12
numgauss=4
totgauss=16
realign_iters="1 2 3 4 5 6 7 9 11 13 15 17 21 23";
88.2%
```

```
numiters=25
maxiterinc=16
numgauss=4
totgauss=20
realign_iters="1 2 3 4 5 6 7 9 11 13 15 17 21 23";
88.2%
```

```
numiters=25
maxiterinc=15
numgauss=1
totgauss=16
realign_iters="1 2 3 4 5 6 7 9 11 13 15 17 21 23";
88.2%
```

```
numiters=25
maxiterinc=9
numgauss=1
totgauss=5
realign_iters="1 2 3 4 5 6 7 9 11 13 15 17 21 23";
88.2%
```

Adjust test_beam

```
numiters=20
maxiterinc=5
numgauss=3
totgauss=8
realign_iters="1 2 3 4 5 6 7 9 11 13 15 18";
88.31%
```

15.0 -> 88.31%

5 -> 64.02%

### 90.73%

```
### parameters that you can modify
numiters=15                                    # Number of iterations of training
maxiterinc=10                                  # Last iter to increase #Gauss on.
numgauss=20                                    # Initial num-Gauss (must be more than #states=3*phones).
totgauss=60                                    # Target #Gaussians.
incgauss=$[($totgauss-$numgauss)/$maxiterinc] # per-iter increment for #Gauss
realign_iters="1 2 3 4 5 6 7 11";
scale_opts="--transition-scale=2.0 --acoustic-scale=1.5 --self-loop-scale=1.2"
###
### parameters that you can modify
opt_acwt=0.125
test_beam=20.0
###
```

### 92.775%

```
### parameters that you can modify
numiters=17                                    # Number of iterations of training
maxiterinc=10                                  # Last iter to increase #Gauss on.
numgauss=20                                    # Initial num-Gauss (must be more than #states=3*phones).
totgauss=100                                    # Target #Gaussians.
incgauss=$[($totgauss-$numgauss)/$maxiterinc] # per-iter increment for #Gauss
realign_iters="1 2 3 4 5 6 7 11";
scale_opts="--transition-scale=2.0 --acoustic-scale=1.5 --self-loop-scale=1.2"
###
```

```
### parameters that you can modify
opt_acwt=0.125
test_beam=20.0
###
```

### 93.67%

```
### parameters that you can modify
numiters=19                                    # Number of iterations of training
maxiterinc=10                                  # Last iter to increase #Gauss on.
numgauss=20                                    # Initial num-Gauss (must be more than #states=3*phones).
totgauss=200                                    # Target #Gaussians.
incgauss=$[($totgauss-$numgauss)/$maxiterinc] # per-iter increment for #Gauss
realign_iters="1 2 3 4 5 6 7 11";
scale_opts="--transition-scale=2.0 --acoustic-scale=1.5 --self-loop-scale=1.2"
###
opt_acwt=0.125
test_beam=20.0
```

0.1 -> 93.26%
0.125 -> 93.67%
0.175 -> 93.9%
0.18 -> 93.96%
0.19 -> 93.9%
0.195 -> 93.84%
0.2 -> 93.78%
0.205 -> 93.78%
0.21 -> 93.61%
0.25 -> 93.03%
0.3 -> 92.65%

## 94.88%

```
### parameters that you can modify
numiters=18                                    # Number of iterations of training
maxiterinc=10                                  # Last iter to increase #Gauss on.
numgauss=20                                    # Initial num-Gauss (must be more than #states=3*phones).
totgauss=300                                    # Target #Gaussians.
incgauss=$[($totgauss-$numgauss)/$maxiterinc] # per-iter increment for #Gauss
realign_iters="1 2 3 4 5 6 7 11";
scale_opts="--transition-scale=2.0 --acoustic-scale=1.5 --self-loop-scale=1.2"
###

### parameters that you can modify
opt_acwt=0.18
test_beam=20.0
###
```

numiters=17

transition-scale
1.0 -> 94.59%
1.5 -> 94.53%
2.0 -> 94.88%
2.25 -> 94.53%
2.4 -> 94.59%
2.5 -> 94.65%
3.0 -> 94.47%

acoustic-scale
1.3 -> 94.47%
1.4 -> 94.88%
1.45 -> 94.75%
1.5 -> 94.88%
1.6 -> 94.65%
2.0 -> 94.53%

self-loop-scale
1.0 -> 94.59%
1.2 -> 94.88%
1.5 -> 94.88%
1.8 -> 94.7%

```
### parameters that you can modify
numiters=17                                    # Number of iterations of training
maxiterinc=10                                  # Last iter to increase #Gauss on.
numgauss=20                                    # Initial num-Gauss (must be more than #states=3*phones).
totgauss=500                                    # Target #Gaussians.
incgauss=$[($totgauss-$numgauss)/$maxiterinc] # per-iter increment for #Gauss
realign_iters="1 2 3 4 5 6 7 11";
scale_opts="--transition-scale=2.0 --acoustic-scale=1.5 --self-loop-scale=1.5"
###
opt_acwt=0.18
test_beam=20.0
```

```
96.2%
### parameters that you can modify
numiters=17                                    # Number of iterations of training
maxiterinc=10                                  # Last iter to increase #Gauss on.
numgauss=80                                    # Initial num-Gauss (must be more than #states=3*phones).
totgauss=900                                    # Target #Gaussians.
incgauss=$[($totgauss-$numgauss)/$maxiterinc] # per-iter increment for #Gauss
realign_iters="1 2 3 4 5 6 7 8 9 10 11 12 13 14";
scale_opts="--transition-scale=2.0 --acoustic-scale=1.5 --self-loop-scale=1.5"
###
opt_acwt=0.18
test_beam=20.0
###
```

```
96.66%
numiters=17                                    # Number of iterations of training
maxiterinc=10                                  # Last iter to increase #Gauss on.
numgauss=80                                    # Initial num-Gauss (must be more than #states=3*phones).
totgauss=6000                                    # Target #Gaussians.
incgauss=$[($totgauss-$numgauss)/$maxiterinc] # per-iter increment for #Gauss
realign_iters="1 2 3 4 5 6 7 8 9 10 11 12 13 14";
scale_opts="--transition-scale=2.0 --acoustic-scale=1.5 --self-loop-scale=1.5"
###
```

```
97.24%
### parameters that you can modify
numiters=17                                    # Number of iterations of training
maxiterinc=10                                  # Last iter to increase #Gauss on.
numgauss=80                                    # Initial num-Gauss (must be more than #states=3*phones).
totgauss=6000                                    # Target #Gaussians.
incgauss=$[($totgauss-$numgauss)/$maxiterinc] # per-iter increment for #Gauss
realign_iters="1 2 3 4 5 6 7 8 9 10 11 12 13 14";
scale_opts="--transition-scale=2.0 --acoustic-scale=1.5 --self-loop-scale=1.5"
###
opt_acwt=0.18
test_beam=20.0
```

totgauss

5000 -> 97.24%
6000 -> 97.29%
7000 -> 96.95%

realign_iters
9(X) -> 97.47%
11(X) -> 97.41%
12(X) -> 97.52%
13(X) -> 97.35%
5(X) -> 97.24%

---

```
95.39%
<State> 0 <PdfClass> 0 <Transition> 0 0.75 <Transition> 1 0.25 </State>
<State> 1 <PdfClass> 1 <Transition> 1 0.75 <Transition> 2 0.25 </State>
<State> 2 <PdfClass> 2 <Transition> 2 0.75 <Transition> 3 0.25 </State>
<State> 3 <PdfClass> 3 <Transition> 3 0.75 <Transition> 4 0.25 </State>
<State> 4 <PdfClass> 4 <Transition> 4 0.75 <Transition> 5 0.25 </State>
<State> 5 </State>
</TopologyEntry>
<TopologyEntry>
<ForPhones>
SILENCEPHONES
</ForPhones>
<State> 0 <PdfClass> 0 <Transition> 0 0.50 <Transition> 1 0.50 </State>
<State> 1 <PdfClass> 1 <Transition> 1 0.50 <Transition> 2 0.50 </State>
<State> 2 <PdfClass> 2 <Transition> 2 0.50 <Transition> 3 0.50 </State>
<State> 3 <PdfClass> 3 <Transition> 3 0.50 <Transition> 4 0.50 </State>
<State> 4 <PdfClass> 4 <Transition> 4 0.50 <Transition> 5 0.50 </State>
<State> 5 </State>
```

```
95.62%
<Topology>
<TopologyEntry>
<ForPhones>
NONSILENCEPHONES
</ForPhones>
<State> 0 <PdfClass> 0 <Transition> 0 0.75 <Transition> 1 0.25 </State>
<State> 1 <PdfClass> 1 <Transition> 1 0.75 <Transition> 2 0.25 </State>
<State> 2 <PdfClass> 2 <Transition> 2 0.75 <Transition> 3 0.25 </State>
<State> 3 <PdfClass> 3 <Transition> 3 0.75 <Transition> 4 0.25 </State>
<State> 4 <PdfClass> 4 <Transition> 4 0.75 <Transition> 5 0.25 </State>
<State> 5 </State>
</TopologyEntry>
<TopologyEntry>
<ForPhones>
SILENCEPHONES
</ForPhones>
<State> 0 <PdfClass> 0 <Transition> 1 0.50 <Transition> 2 0.50 </State>
<State> 1 <PdfClass> 1 <Transition> 1 0.75 <Transition> 2 0.25 </State>
<State> 2 <PdfClass> 2 <Transition> 2 0.25 <Transition> 3 0.75 </State>
<State> 3 </State>
</TopologyEntry>
</Topology>

```

```
94.59%
NONSILENCEPHONES
</ForPhones>
<State> 0 <PdfClass> 0 <Transition> 0 0.75 <Transition> 1 0.25 </State>
<State> 1 <PdfClass> 1 <Transition> 1 0.75 <Transition> 2 0.25 </State>
<State> 2 <PdfClass> 2 <Transition> 2 0.75 <Transition> 3 0.25 </State>
<State> 3 </State>
</TopologyEntry>
<TopologyEntry>
<ForPhones>
SILENCEPHONES
</ForPhones>
<State> 0 <PdfClass> 0 <Transition> 1 0.50 <Transition> 2 0.50 </State>
<State> 1 <PdfClass> 1 <Transition> 1 0.75 <Transition> 2 0.25 </State>
<State> 2 <PdfClass> 2 <Transition> 2 0.25 <Transition> 3 0.75 </State>
<State> 3 </State>
</TopologyEntry>
```

```
96.14%
<State> 0 <PdfClass> 0 <Transition> 0 0.75 <Transition> 1 0.25 </State>
<State> 1 <PdfClass> 1 <Transition> 1 0.75 <Transition> 2 0.25 </State>
<State> 2 <PdfClass> 2 <Transition> 2 0.75 <Transition> 3 0.25 </State>
<State> 3 <PdfClass> 3 <Transition> 3 0.75 <Transition> 4 0.25 </State>
<State> 4 <PdfClass> 4 <Transition> 4 0.75 <Transition> 5 0.25 </State>
<State> 5 </State>
</TopologyEntry>
<TopologyEntry>
<ForPhones>
SILENCEPHONES
</ForPhones>
<State> 0 <PdfClass> 0 <Transition> 1 0.50 <Transition> 2 0.50 </State>
<State> 1 <PdfClass> 1 <Transition> 2 0.75 <Transition> 3 0.25 </State>
<State> 2 <PdfClass> 2 <Transition> 2 0.25 <Transition> 3 0.75 </State>
<State> 3 </State>
```

totgauss
3000 -> 97.18%
4000 -> 97.41%
4200 -> 97.35%
5000 -> 97.29%
