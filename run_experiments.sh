#!/bin/bash

echo "-- Compiling --"
make

echo "-- Simulating the critical path for scenarios 1 and 2 --"
./engine_simulator input_CP_scenario_1.txt
mv Result/ Result_CP_1/

echo "-- Simulating scenario 1 --"
./engine_simulator input_scenario_1.txt
mv Result/ Result_1/

echo "-- Simulating scenario 2 --"
./engine_simulator input_scenario_2.txt
mv Result/ Result_2/

echo "-- Simulating the critical path for scenario 3 --"
./engine_simulator input_CP_scenario_3.txt
mv Result/ Result_CP_3/

echo "-- Simulating scenario 3 --"
./engine_simulator input_scenario_3.txt
mv Result/ Result_3/
