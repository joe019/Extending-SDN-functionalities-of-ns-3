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