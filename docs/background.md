---
title: Background
layout: default
---

<h2>ns-3</h2>
<p> ns-3 is a discrete-event network simulator. If, for example, an event scheduled for simulation time "100 seconds" is executed, and the next event is not scheduled until "200 seconds", the simulator will immediately jump from 100 seconds to 200 seconds (of simulation time) to execute the next event. This is what is meant by "discrete-event" simulator. </p>

<h2>SDN</h2>
<p>SDN (Software Defined Networking) refers to the physical separation of the network control plane from the forwarding plane, and where a control plane controls several devices.
<ul>
	<li>Data plane refers to all the functions and processes that forward packets/frames from one interface to another.</li>
	<li>Control plane refers to all the functions and processes that determine which path to use. Routing protocols, spanning tree, ldp, etc are examples.</li>
</ul>
The control plane consists of one or more controllers which are considered as the brain of SDN network, where the whole intelligence is incorporated. The below figure depicts the architecture of SDN:</p>
<div class="row content-row">
	<div class="col-12 col-sm-4"></div>
	<div class="col-12 col-sm-4">
	    <img src="{{ site.baseurl }}/images/sdnArch.png">
	</div>
	<div class="col-12 col-sm-4"></div>
</div>

<h2>OpenFlow</h2>
<p>OpenFlow is considered one of the first software-defined networking standards. It originally defined the communication protocol in SDN environments that enables an SDN controller to directly interact with the forwarding plane of network devices such as switches and routers, both physical and virtual (hypervisor-based).
OpenFlow:
<ul>
	<li>is a communications protocol that gives access to the forwarding plane of a network switch or router over the network.</li>
	<li>enables network controllers to determine the path of network packets across a network of switches.</li>
	<li>allows switches from different vendors - often each with their own proprietary interfaces and scripting languages - to be managed remotely using a single, open protocol.</li>
</ul>

<h2 id="ns-3">Getting started with OpenFlow in ns-3</h2>

<h3>1. Download ns-3</h3>

<p>If mercurial is installed on your system, you can download ns-3 directly:</p>

<code>hg clone http://code.nsnam.org/ns-3-dev</code><br>

<p>To obtain additional related libraries such as support for Python bindings, clone the ‘ns-3-allinone’ repository as follows, and run the download.py program:</p>

<code>hg clone http://code.nsnam.org/ns-3-allinone</code> <br>
<code>cd ns-3-allinone && ./download.py</code> <br>

<h3>2. Configure ns-3</h3>

<p>Change the current directory to ns-3-allinone and run the following:</p>

<code>./build.py</code> <br>
<code>cd ns-3-dev</code> <br>
<code>./waf configure --enable-examples --enable-tests</code> <br>

<p>To run a sample program:</p>

<code>./waf --run  first</code>

<h3>3. Enable Openflow integration:</h3>

<p>In order to use the OpenFlowSwitch module, you must create and link the OFSID (OpenFlow Software Implementation Distribution) to ns-3. To do this:</p>

<p>Obtain the OFSID code. An ns-3 specific OFSID branch is provided to ensure operation with ns-3. Use mercurial to download this branch and waf to build the library:</p>

<code>hg clone http://code.nsnam.org/openflow</code><br>
<code>cd openflow</code>

<p>From the “openflow” directory, run:</p>

<code>./waf configure</code><br>
<code>./waf build</code>

<p>Your OFSID is now built into a libopenflow.a library! To link to an ns-3 build with this OpenFlow switch module, run from the ns-3-dev:</p>

<code>./waf configure --enable-examples --enable-tests --with-openflow=pathToOpenflow</code><br>
where <code>pathToOpenflow=/home/gian/openflow or /home/joe019/7th\ sem/projects/openflow</code><br>

<p><code>Under ---- Summary of optional NS-3 features:</code> you should see:</p>

<code>"NS-3 OpenFlow Integration     : enabled"</code>

<p>indicating the library has been linked to ns-3. Run:</p>

<code>./waf build</code><br>

<p>to build ns-3 and activate the OpenFlowSwitch module in ns-3.</p>

<p>For an example demonstrating its use in a simple learning controller/switch, run:</p>

<code>./waf --run openflow-switch</code><br>

<p>To see it in detailed logging, run:</p>

<code>./waf --run "openflow-switch -v"</code><br>
</ol>