# IBM PowerVP Monitoring Framework

This repository contains the source code of an adaptive application prototype leveraging performance counters from IBM PowerVP. It was built for the [Resource Management on Power](https://www.dcl.hpi.uni-potsdam.de/teaching/remapsem/) seminar at HPI.

It consists of the following components:

 - SAP HANA database (`database-schema`)
 - Python tool to record performance counters from the PowerVP GUI into the database (`powervp-adapter`)
 - SAP XSJS Web Application to display the most recent measurements and to display a 'Red Button' whenever a threshold has been exceeded (`monitor`)
 - Benchmark to artificially raise the Remote L4 CPI counter (`pointer-chaser`)

Furthermore this repository contains the following tools:
 - Script to intercept the `perf` calls of a PowerVP agent on Linux for later inspection using a parser (`perf-monitor`)
 - Tool to perform hypercalls using the PowerVP driver (`hypercall`)

This seminar's report can be found [here](report/powervp.pdf).
