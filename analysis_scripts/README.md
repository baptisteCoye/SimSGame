# Performance results of different scheduling algorithms used in the simulation of a modern game engine

These scripts are a companion to the paper entitled "_Exploring scheduling algorithms for parallel task graphs: a modern game engine case study_" by M. Regragui et al.

## General information

This scripts can be used to analyse the results from the simulator.
Result analysis can be directly reproduced using the script `run_analysis.sh`. A series of Jupyter Notebook files are also available to help visualize the results.
If one does not want to rerun the experiments, a copy of the generated dataset is available on [Zenodo](https://zenodo.org/record/6532252).

## File information

- All `Scenario*.ipynb` files contain python scripts to visualize and analyze the simulation results.
- The `Scenario*.py` files contain python scripts that can be run directly with Jupyter Notebook.
- The `requirements.txt` file contains the names and versions of python packages necessary to reproduce the analysis.
- The `run_analysis.sh` file contains a bash script to install the required python packages and run the `Scenario*.py` scripts.

The results are organized in five folders:

1. `Result_1` contains the results for Scenario 1 generated using file `input_scenario_1.txt`.
2. `Result_2` contains the results for Scenario 2 generated using file `input_scenario_2.txt`.
3. `Result_3` contains the results for Scenario 3 generated using file `input_scenario_3.txt`.
4. `Result_CP_1` contains the results for the critical path of Scenarios 1 and 2 generated using file `input_CP_scenario_1.txt`.
5. `Result_CP_3` contains the results for the critical path of Scenario 3 generated using file `input_CP_scenario_3.txt`.

Each result file (e.g., `HLF_NonSorted_Random_1_200_10.txt`) contains 200 lines representing information of the 200 frames that were simulated. Each line contains four values: the frame number, the duration of the frame (in microseconds), a critical path estimation for the previous frame (in microseconds), and the load parameter (value between 0 and 1).

The outputs of this analysis include some PDF files representing the figures in the paper (in order) and some CSV files representing the values shown in tables. The standard output shows the p-values computed in parts of the statistical analysis.

## Software and hardware information

The simulation results were generated on an Intel Core i7-1185G7 processor, with 32 GB of LPDDR4 RAM (3200 MHz). The machine ran on Ubuntu 20.04.3 LTS (5.14.0-1034-oem), and g++ 9.4.0 was used for the simulator's compilation (-O3 flag).

The results were analyzed using Python 3.8.10, pip 20.0.2 and jupyter-notebook 6.0.3. The following packages and their respective versions were used:

- pandas 1.3.2
- numpy 1.21.2
- matplotlib 3.4.3
- seaborn 0.11.2
- scipy 1.7.1
- pytz 2019.3
- python-dateutil 2.7.3
- kiwisolver 1.3.2 
- pyparsing 2.4.7 
- cycler 0.10.0 
- Pillow 7.0.0
- six 1.14.0 

## Simulation information

Simulation results were generated from 4 to 20 resources. Each configuration was run with 50 different RNG seeds (1 up to 50).

Each simulation is composed of 200 frames. The load parameter (lag) starts at zero and increases by 0.01 with each frame up to a value equal to 100% in frame 101. After that, the load parameter starts to decrease in the same rhythm down to 0.01 in frame 200.

## Algorithms abbreviation in presentation order

FIFO serves as the baseline for comparisons.

1. **FIFO:** First In First Out.
2. **LPT:** Longest Processing Time First.
3. **SPT:** Shortest Processing Time First.
4. **SLPT:** LPT at a subtask level.
5. **SSPT:** SPT at a subtask level.
6. **HRRN:** Highest Response Ratio Next. 
7. **WT:** Longest Waiting Time First.
8. **HLF:** Hu's Level First with unitary processing time of each task.
9. **HLFET:** HLF with estimated times.
10. **CG:** Coffman-Graham's Algorithm.
11. **DCP:** Dynamic Critical Path Priority.

## Metrics

* SF: slowest frame (maximum frame execution time)
* DF: number of delayed frames (with 16.667 ms as the due date)
* CS: cumulative slowdown (with 16.667 ms as the due date)

