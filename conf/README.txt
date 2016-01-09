# README.txt in conf directory
# @author : rete
# @date : 07/01/2016


# INTRODUCTION
##############

README file to run a simple dqm setup with the examples provided by 
the framework, locally on your machine. The whole chain will :

 * Publish events
 * Collect event
 * Query and process events
 * Produce and publish monitor elements
 * Collect monitor elements
 * Query and display monitor elements on a gui

It is recommended to have installed the Viz part of the software to 
go quicker in simple tutorial. You must be able to run :

 * dns, th executable provided by dim to start a dim dns node on a 
machine
 * dqm4hep_start_event_collector, to collect events
 * dqm4hep_start_monitor_element_collector, to collector histograms
 * dqm4hep_start_lcio_file_service, your lcio file just have to 
support the available collection streamers. The event itself is not 
processed by the example
 * dqm4hep_start_analysis_module, to start an analysis module (event 
processor)
 * dqm4hep_start_standalone_module, to start a standalone module (
custom data source)
 * dqm4hep_start_run_control (VIZ), to manage a run control server (
start/stop runs)
 * dqm4hep_start_job_control (VIZ), to manage your processes on your
machine.
 * dqm4hep_start_dqm_monitoring_gui (VIZ), to vizualize monitor 
elements at the end of the chain

# PREPARE YOUR SYSTEM
#####################

1) start dim dns node (dim package) : 

$ dns &

2) start dimjc_daemon (dimjc package) as root user

$ sudo su
$ dimjc_daemon &

3) edit full_test_local.json as you need. To obtain your host name, 
type :

$ hostname

in a terminal. Ajust the paths and config files as you need

# RUNNING THE SETUP
###################

From now, in each terminal you open, you must export DIM_DNS_NODE 
to 'localhost' (we are running locally) before running anything else :

$ export DIM_DNS_NODE=localhost

4) Start the job control :

$ dqm4hep_start_job_interface &

Load you json file with your local settings, you've just edited and
start all the jobs. Update the interface to see the status of your 
jobs

If everything is ok, at this step you have :

 * An event service that publishes lcio event
 * An event collector that collects these events
 * An analysis module, waiting for a new run
 * A standalone module, producing and publishing monitor elements
 * A monitor element collector that collects element coming from both 
modules

5) Start a run control : 

$ dqm4hep_start_run_control [-r YourRunControlName] &

If no '-r' option is provided the run control is called "DEFAULT"
The name must match the one provided in your analysismodule settings 
file.

Customize as you the run parameters (not mandatory) and start a new
run.

6) Start a monitor gui :

$ dqm4hep_start_dqm_monitoring_gui &

Click on 'Browse' and select our monitor element collector in the list.
You'll see some elements the in the search result view. Select the one
you wanna monitor and click on "Append" or "Replace" (no matter here).
Tadaaa !
On the main window, some contents appear on the left. You can start 
browsing it by clicking, using the enter key or by drag-and-dropping
element in the canvas area on the right.

Enjoy !







