#!/usr/bin/env python
# coding: utf-8

# # Scenario 1 - tables and plots for the paper
#
# This document contains an advanced analysis of the results obtained with our scheduling simulator.
# It resumes the tables and plots that we are using on our paper.

# **General information**
#
# These simulation results were generated from 4 to 20 resources. Each configuration was run with 50 different RNG seeds (1 up to 50).
#
# Each simulation is composed of 200 frames. Lag starts at zero and increases by 0.01 with each frame up to a lag equal to 100% in frame 101. After that, the lag starts to decrease in the same rhythm down to 0.01 in frame 200.
#
#
# **Algorithms abbreviation in presentation order:**
#
# FIFO serves as the baseline for comparisons.
#
# 1. **FIFO:** First In First Out.
# 2. **LPT:** Longest Processing Time First.
# 3. **SPT:** Shortest Processing Time First.
# 4. **SLPT:** LPT at a subtask level.
# 5. **SSPT:** SPT at a subtask level.
# 6. **HRRN:** Highest Response Ratio Next.
# 7. **WT:** Longest Waiting Time First.
# 8. **HLF:** Hu's Level First with unitary processing time of each task.
# 9. **HLFET:** HLF with estimated times.
# 10. **CG:** Coffman-Graham's Algorithm.
# 11. **DCP:** Dynamic Critical Path Priority.
#
# **Metrics:**
# * SF: slowest frame (maximum frame execution time)
# * DF: number of delayed frames (with 16.667 ms as the due date)
# * CS: cumulative slowdown (with 16.667 ms as the due date)

# In[ ]:


import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats


# ### Table I
# **Average values for the different metrics and algorithms with 12 resources**
#

# In[ ]:


# Constants
seeds = [str(i) for i in range(1,51)]
algorithms = ["FIFO", "LPT", "SPT", "SLRT", "SSRT", "HRRN", "WT", "HLF", "Hu", "Coffman", "Priority"]
finalnames = ["FIFO", "LPT", "SPT", "SLPT", "SSPT", "HRRN", "WT", "HLF", "HLFET", "CG", "DCP"]
resources = [i for i in range (4,21)]

directory = "../Result_1/"
directory_cp = "../Result_CP_1/"

colors = sns.color_palette('hls', n_colors=13)
sns.set_style("whitegrid")


# In[ ]:


# Function to generate the path and file name
def file_name_and_path(directory, algorithm, resources, seed):
    filename = directory + algorithm + "/" + resources + "/200/TXT/" +                algorithm + "_NonSorted_Random_" + seed + "_200_" + resources + ".txt"
    return filename


# In[ ]:


# Function to generate tables with average results
def generate_table(algorithms, names, seeds, directory, resources):
    sf = []
    df = []
    cs = []
    for algo in algorithms:
        accum_sf = []
        accum_df = []
        accum_cs = []
        for s in seeds:
            filename = file_name_and_path(directory, algo, resources, s)
            data = pd.read_csv(filename, sep=' ', header=None)
            accum_sf.append(np.max(data[1]))
            accum_df.append(len([x for x in data[1] if x > 16667]))
            accum_cs.append(sum([x - 16667 for x in data[1] if x > 16667]))
        sf.append(np.mean(accum_sf)/1000)
        df.append(np.mean(accum_df))
        cs.append(np.mean(accum_cs)/1000)

    data = {'Algorithms':names, 'SF (ms)':sf, 'DF (frames)':df, 'CS (ms)':cs}
    return pd.DataFrame(data).transpose()


# In[ ]:


table = generate_table(algorithms, finalnames, seeds, directory, '12')
table.to_csv('../table_2___average_metrics_12_resources.csv', header=False)


# In[ ]:


generate_table(algorithms, finalnames, seeds, directory, '4')


# In[ ]:


generate_table(algorithms, finalnames, seeds, directory, '12')


# In[ ]:


generate_table(algorithms, finalnames, seeds, directory, '20')


# ## Figure 1
# **Boxplots of the different metrics for all algorithms and 12 resources**

# In[ ]:


metrics = ['SF', 'DF', 'CS']


# In[ ]:


# Function to gather data in one dataframe per number of resources
def gather_all_data(algorithms, names, seeds, directory, resources):
    k = []
    for i in range(len(algorithms)):
        for s in seeds:
            filename = file_name_and_path(directory, algorithms[i], str(resources), s)
            df = pd.read_csv(filename, sep=' ', header=None)
            k1 = []
            k1.append(names[i])
            k1.append(s)
            k1.append(np.max(df[1])/1000)
            k1.append(len([x for x in df[1] if x > 16667]))
            k1.append(sum([x - 16667 for x in df[1] if x > 16667])/1000)
            k.append(k1)

    df = pd.DataFrame(k)
    df.columns = ['Algorithm', 'Seed', 'SF', 'DF', 'CS']
    return df


# In[ ]:


data = gather_all_data(algorithms, finalnames, seeds, directory, 12)
data


# In[ ]:


fontsize = 15


# In[ ]:


plt.figure(figsize=(6,5))
plt.rcParams['axes.titlesize'] = fontsize
plt.rcParams['axes.labelsize'] = fontsize
plt.rcParams['xtick.labelsize'] = fontsize
plt.rcParams['ytick.labelsize'] = fontsize

plt.xticks(rotation=45)
plt.ylim(26, 40)

sns.boxplot(x=data['Algorithm'], y=data[metrics[0]], data=data, palette=colors)
plt.ylabel('Slowest Frame (ms)', fontsize=fontsize)
plt.xlabel('Scheduling algorithms', fontsize=fontsize)

plt.savefig("../results-1-sf.pdf", bbox_inches='tight')


# In[ ]:


plt.figure(figsize=(6,5))
plt.rcParams['axes.titlesize'] = fontsize
plt.rcParams['axes.labelsize'] = fontsize
plt.rcParams['xtick.labelsize'] = fontsize
plt.rcParams['ytick.labelsize'] = fontsize

plt.xticks(rotation=45)
plt.ylim(59, 85)

sns.boxplot(x=data['Algorithm'], y=data[metrics[1]], data=data, palette=colors)
plt.ylabel('Delayed Frames (frames)', fontsize=fontsize)
plt.xlabel('Scheduling algorithms', fontsize=fontsize)

plt.savefig("../results-1-df.pdf", bbox_inches='tight')


# In[ ]:


plt.figure(figsize=(6,5))
plt.rcParams['axes.titlesize'] = fontsize
plt.rcParams['axes.labelsize'] = fontsize
plt.rcParams['xtick.labelsize'] = fontsize
plt.rcParams['ytick.labelsize'] = fontsize

plt.xticks(rotation=45)
plt.ylim(270, 450)

sns.boxplot(x=data['Algorithm'], y=data[metrics[2]], data=data, palette=colors)
plt.ylabel('Cumulative Slowdown (ms)', fontsize=fontsize)
plt.xlabel('Scheduling algorithms', fontsize=fontsize)

plt.savefig("../results-1-cs.pdf", bbox_inches='tight')


# ## Statistics
#
# 1. Comparisons between FIFO and LPT, WT, CG, and DCP
# 2. Comparisons between WT, CG, and DCP

# In[ ]:


# KS tests - applied for each algorithm and each metric
algos = ['FIFO', 'LPT', 'WT', 'CG', 'DCP']
sfs = {}
dfs = {}
css = {}

np.random.seed(2021) # setting RNG seed for analysis

# preparing the lists of values
for algo in algos:
    sfs[algo] = list(data.loc[(data['Algorithm'] == algo)].SF)
    dfs[algo] = list(data.loc[(data['Algorithm'] == algo)].DF)
    css[algo] = list(data.loc[(data['Algorithm'] == algo)].CS)

# running the KS test
for algo in algos:
    print(f'KS test for scheduler {algo} and the Slowest Frame metric')
    results = sfs[algo]
    print(stats.kstest(results, 'norm', args=(np.mean(results), np.std(results))))
    print(f'\nKS test for scheduler {algo} and the Delayed Frames metric')
    results = dfs[algo]
    print(stats.kstest(results, 'norm', args=(np.mean(results), np.std(results))))
    print(f'\nKS test for scheduler {algo} and the Cumulative Slowdown metric')
    results = css[algo]
    print(stats.kstest(results, 'norm', args=(np.mean(results), np.std(results))))
    print('-----------')


# No p-values under 0.05, so we cannot reject the null hypothesis that these results come from normal distributions.
# We will then apply an F-test to see if pairs of results have the same variance.

# In[ ]:


# F-test function
def f_test(x, y):
    x = np.array(x)
    y = np.array(y)
    f = np.var(x, ddof=1)/np.var(y, ddof=1) #calculate F test statistic
    dfn = x.size - 1 #define degrees of freedom numerator
    dfd = y.size - 1 #define degrees of freedom denominator

    p = 1 - stats.f.cdf(f, dfn, dfd) #find p-value of F test statistic
    return f, p


# In[ ]:


def run_f_tests(standard, algos, sfs, dfs, css):
    for algo in algos:
        print(f'F-test for schedulers {standard} and {algo} and the Slowest Frame metric')
        F, p_value = f_test(sfs[standard], sfs[algo])
        print(f'p-value = {p_value}')
        print(f'F-test for schedulers {standard} and {algo} and the Delayed Frames metric')
        F, p_value = f_test(dfs[standard], dfs[algo])
        print(f'p-value = {p_value}')
        print(f'F-test for schedulers {standard} and {algo} and the Cumulative Slowdown metric')
        F, p_value = f_test(css[standard], css[algo])
        print(f'p-value = {p_value}')
        print(' ')
    print('-----------')

# Comparing the variances for FIFO and other schedulers
run_f_tests('FIFO', ['LPT', 'WT', 'CG', 'DCP'], sfs, dfs, css)

# Comparing variances for WT and other schedulers
run_f_tests('WT', ['CG', 'DCP'], sfs, dfs, css)

# Comparing variances for CG and other schedulers
run_f_tests('CG', ['DCP'], sfs, dfs, css)


# No p-values under 0.05, so it seems our variances are all equal.
# We can move to standard t-tests in this case.

# In[ ]:


def run_t_tests(standard, algos, sfs, dfs, css):
    for algo in algos:
        print(f'T-test for schedulers {standard} and {algo} and the Slowest Frame metric')
        F, p_value = stats.ttest_rel(sfs[standard], sfs[algo])
        print(f'p-value = {p_value}')
        print(f'T-test for schedulers {standard} and {algo} and the Delayed Frames metric')
        F, p_value = stats.ttest_rel(dfs[standard], dfs[algo])
        print(f'p-value = {p_value}')
        print(f'T-test for schedulers {standard} and {algo} and the Cumulative Slowdown metric')
        F, p_value = stats.ttest_rel(css[standard], css[algo])
        print(f'p-value = {p_value}')
        print(' ')
    print('-----------')

# T-test for FIFO and other schedulers
run_t_tests('FIFO', ['LPT', 'WT', 'CG', 'DCP'], sfs, dfs, css)

# T-test for WT and other schedulers
run_t_tests('WT', ['CG', 'DCP'], sfs, dfs, css)

# T-test for CG and other schedulers
run_t_tests('CG', ['DCP'], sfs, dfs, css)


# Resumed view of these results:
#
# 1. Besides LPT and the SF metric, all compared schedulers provide results that are different than FIFO (p-values < 0.05).
# 2. WT is only different to CG and DCP in the CS metric. This is also the case between CG and DCP.

# ## Figure 2
# **Histograms of frame duration reduction when compared to the baseline (FIFO)**

# In[ ]:


# Function to gather data in one dataframe per number of resources and algorithm
def gather_hist_data(algorithms, names, seeds, directory, r):
    results = {}  # dictionary per algorithm
    for i in range(len(algorithms)):
        if algorithms[i] == "FIFO": # nothing to do for FIFO here
            continue
        results[names[i]] = {}  # dictionary per number of resources
        k = []
        for s in seeds:
            filename = file_name_and_path(directory, algorithms[i], str(r), s)
            df = pd.read_csv(filename, sep=' ', header=None)
            fifoname = file_name_and_path(directory, "FIFO", str(r), s)
            df_fifo = pd.read_csv(fifoname, sep=' ', header=None)
            k.append([x-y for x,y in zip(df_fifo[1],df[1])])

        flat_list = [item for sublist in k for item in sublist]
        results[names[i]] = pd.DataFrame(flat_list)
    return results


# In[ ]:


hist_data = gather_hist_data(algorithms, finalnames, seeds, directory, 12)


# In[ ]:


hist_algos = ['LPT', 'WT', 'CG', 'DCP']

# Function that returns the maximum and minimum values for a given algorithm
def max_and_min(data, algorithms):
    maximums = []
    minimums = []
    for algo in algorithms:
        maximums.append(max(data[algo][0]))
        minimums.append(min(data[algo][0]))
    return max(maximums), min(minimums)


# In[ ]:


max_and_min(hist_data, hist_algos)


# In[ ]:


# Plots the histrograms

plt.figure(figsize=(4,4))
plt.rcParams['axes.titlesize'] = fontsize
plt.rcParams['axes.labelsize'] = fontsize
plt.rcParams['xtick.labelsize'] = fontsize
plt.rcParams['ytick.labelsize'] = fontsize

plt.xticks(rotation=25)
plt.xlim(-1000, 1000)
plt.ylim(0, 650)

sns.histplot(hist_data[hist_algos[0]], legend=False,  palette=[colors[1]], binwidth=20, kde=True)
plt.ylabel('Frequency', fontsize=fontsize)
plt.xlabel('Frame duration reduction vs. FIFO', fontsize=fontsize)

plt.savefig("../results-2-lpt.pdf", bbox_inches='tight')


# In[ ]:


plt.figure(figsize=(4,4))
plt.rcParams['axes.titlesize'] = fontsize
plt.rcParams['axes.labelsize'] = fontsize
plt.rcParams['xtick.labelsize'] = fontsize
plt.rcParams['ytick.labelsize'] = fontsize

plt.xticks(rotation=25)
plt.xlim(-1000, 1000)
plt.ylim(0, 650)

sns.histplot(hist_data[hist_algos[1]], legend=False, palette=[colors[6]], binwidth=20, kde=True)
plt.ylabel('Frequency', fontsize=fontsize)
plt.xlabel('Frame duration reduction vs. FIFO', fontsize=fontsize)

plt.savefig("../results-2-wt.pdf", bbox_inches='tight')


# In[ ]:


plt.figure(figsize=(4,4))
plt.rcParams['axes.titlesize'] = fontsize
plt.rcParams['axes.labelsize'] = fontsize
plt.rcParams['xtick.labelsize'] = fontsize
plt.rcParams['ytick.labelsize'] = fontsize

plt.xticks(rotation=25)
plt.xlim(-1000, 1000)
plt.ylim(0, 650)

sns.histplot(hist_data[hist_algos[2]], legend=False, palette=[colors[9]], binwidth=20, kde=True)
plt.ylabel('Frequency', fontsize=fontsize)
plt.xlabel('Frame duration reduction vs. FIFO', fontsize=fontsize)

plt.savefig("../results-2-cg.pdf", bbox_inches='tight')


# In[ ]:


plt.figure(figsize=(4,4))
plt.rcParams['axes.titlesize'] = fontsize
plt.rcParams['axes.labelsize'] = fontsize
plt.rcParams['xtick.labelsize'] = fontsize
plt.rcParams['ytick.labelsize'] = fontsize

plt.xticks(rotation=25)
plt.xlim(-1000, 1000)
plt.ylim(0, 650)

sns.histplot(hist_data[hist_algos[3]], legend=False, palette=[colors[10]], binwidth=25, kde=True)
plt.ylabel('Frequency', fontsize=fontsize)
plt.xlabel('Frame duration reduction vs. FIFO', fontsize=fontsize)

plt.savefig("../results-2-dcp.pdf", bbox_inches='tight')


# ### Information from the critical path for comparison

# In[ ]:


# Function to generate tables with average results
def generate_table_cp(algorithms, names, seeds, directory, resources):
    sf = []
    df = []
    cs = []
    for algo in algorithms:
        accum_sf = []
        accum_df = []
        accum_cs = []
        for s in seeds:
            filename = file_name_and_path(directory, algo, resources, s)
            data = pd.read_csv(filename, sep=' ', header=None)
            accum_sf.append(np.max(data[1]))
            accum_df.append(len([x for x in data[1] if x > 16667]))
            accum_cs.append(sum([x - 16667 for x in data[1] if x > 16667]))
        sf.append(np.mean(accum_sf)/1000)
        df.append(np.mean(accum_df))
        cs.append(np.mean(accum_cs)/1000)

    data = {'Algorithms':names, 'SF (ms)':sf, 'DF (frames)':df, 'CS (ms)':cs}
    return pd.DataFrame(data).transpose()


# In[ ]:


cp_data = generate_table_cp(['Infinity'], ['Critical Path'], seeds, directory_cp, '1000')


# ## Figure 3
# **Lineplots showing the evolution of metrics when the number of resources changes**

# In[ ]:


# Function to gather data in one dataframe per number of resources
def gather_all_data(algorithms, names, seeds, directory, resources):
    k = []
    for i in range(len(algorithms)):
        for r in resources:
            for s in seeds:
                filename = file_name_and_path(directory, algorithms[i], str(r), s)
                df = pd.read_csv(filename, sep=' ', header=None)
                k1 = []
                k1.append(names[i])
                k1.append(r)
                k1.append(s)
                k1.append(np.max(df[1])/1000)
                k1.append(len([x for x in df[1] if x > 16667]))
                k1.append(sum([x - 16667 for x in df[1] if x > 16667])/1000)
                k.append(k1)
    df = pd.DataFrame(k)
    df.columns = ['Algorithm', 'Resources', 'Seed', 'SF', 'DF', 'CS']
    return df


# In[ ]:


all_data = gather_all_data(algorithms, finalnames, seeds, directory, resources)
all_data


# In[ ]:


# Plots the boxplots for a given number of resources and metric
def plot_lineplots(data, algorithm, metrics):
    color = {metrics[0]:'blue', metrics[1]:'orange', metrics[2]:'green'}
    for metric in metrics:
        fig,ax = plt.subplots(1,1, figsize=(15,12))
        sns.lineplot(x='Resources', y=metric, ci='sd', data=data[algorithm],
                     marker='o', color=color[metric])
        ax.set(title=metric)


# In[ ]:


plot_data = all_data.loc[all_data['Algorithm'].isin(['FIFO', 'LPT', 'WT', 'CG', 'DCP'])]


# In[ ]:


plot_data


# In[ ]:


plt.figure(figsize=(6,5))
plt.rcParams['axes.titlesize'] = fontsize
plt.rcParams['axes.labelsize'] = fontsize
plt.rcParams['xtick.labelsize'] = fontsize
plt.rcParams['ytick.labelsize'] = fontsize

plt.xticks(rotation=25)
plt.xlim(4, 20)
plt.ylim(40, 140)

sns.lineplot(x='Resources', y='DF', ci=None, data=plot_data, marker='o', hue='Algorithm',
             palette=[colors[0], colors[1], colors[6], colors[9], colors[10]])
plt.ylabel('Average Delayed Frames (frames)', fontsize=fontsize)
sns.lineplot(x=resources, y=17*[cp_data[0][2]], ci=None, color='gray', label='Critical Path')
plt.legend(fontsize=(fontsize-1))
plt.xticks(range(4,21,2))

plt.savefig("../results-3-resources-df.pdf", bbox_inches='tight')


# ## Figure 4
# **Lineplots with the duration of the frames for FIFO, an algorithm, and the critical path**

# In[ ]:


# Function to gather data in one dataframe for one number of resources and algorithm
def gather_line_data(algorithm, seeds, directory, resource):
    data = []
    for s in seeds:
        filename = file_name_and_path(directory, algorithm, resource, s)
        df = pd.read_csv(filename, sep=' ', header=None)
        data += [df]
    result = pd.concat(data)
    return result


# In[ ]:


plt.figure(figsize=(6,5))

plt.rcParams['axes.titlesize'] = fontsize
plt.rcParams['axes.labelsize'] = fontsize
plt.rcParams['xtick.labelsize'] = fontsize
plt.rcParams['ytick.labelsize'] = fontsize

plt.xticks(rotation=25)
plt.xlim(0, 200)
plt.ylim(5, 27.500)

result = gather_line_data('Coffman', seeds, directory, '12')
sns.lineplot(x=list(result[0]), y=list(result[1]/1000), ci=None, label='CG w/ 12 res.', color=colors[9])

result = gather_line_data('Coffman', seeds, directory, '20')
sns.lineplot(x=list(result[0]), y=list(result[1]/1000), ci=None, label='CG w/ 20 res.', color=colors[11])

result = gather_line_data('Infinity', seeds, directory_cp, '1000')
sns.lineplot(x=list(result[0]), y=list(result[1]/1000), ci=None, label='Critical Path', color='gray')

sns.lineplot(x=list(range(1,201)), y=200*[16.667], ci=None, label='16.667 ms', color='red')

plt.xlabel('Frame number', fontsize=fontsize)
plt.ylabel('Average Frame Duration (ms)', fontsize=fontsize)
plt.legend(fontsize=(fontsize-1))
plt.xticks(range(0,201,25))

plt.savefig("../results-4-frame-duration.pdf", bbox_inches='tight')
