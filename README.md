# Extending SDN functionality in ns-3
## Course: Major Project
## Course Code: CO449


## Overview

## 1. What is SDN?
The physical separation of the network control plane from the forwarding plane, and where a control plane controls several devices.

### What is the difference between data plane and control plane?
* Data plane refers to all the functions and processes that forward packets/frames from one interface to another.
* Control plane refers to all the functions and processes that determine which path to use. Routing protocols, spanning tree, ldp, etc are examples.
[...Read more on SDN](https://github.com/joe019/SDN-Simulation/wiki/1.-SDN)


## 2. What is OpenFlow?
OpenFlow:
* is a communications protocol that gives access to the forwarding plane of a network switch or router over the network. 
* enables network controllers to determine the path of network packets across a network of switches. 
* allows switches from different vendors — often each with their own proprietary interfaces and scripting languages — to be managed remotely using a single, open protocol. 

### How does OpenFlow work?
* OpenFlow allows remote administration of a layer 3 switch's packet forwarding tables, by adding, modifying and removing packet matching rules and actions. This way, routing decisions can be made periodically or ad hoc (_done for a particular purpose as necessary_) by the controller and translated into rules and actions with a configurable lifespan, which are then deployed to a switch's flow table, leaving the actual forwarding of matched packets to the switch at wire speed for the duration of those rules. 
* Packets which are unmatched by the switch can be forwarded to the controller. The controller can then decide to modify existing flow table rules on one or more switches or to deploy new rules, to prevent a structural flow of traffic between switch and controller. It could even decide to forward the traffic itself, provided that it has told the switch to forward entire packets instead of just their header.

### How is OpenFlow placed in the protocol stack?
The OpenFlow protocol is layered on top of the Transmission Control Protocol (TCP) and prescribes the use of Transport Layer Security (TLS). 

### What is the standard port for OpenFlow?
Controllers should listen on TCP port 6653 for switches that want to set up a connection. 
[...Read more on OpenFlow](https://github.com/joe019/SDN-Simulation/wiki/2.-OpenFlow)


## 3. What is P4?
P4 is a declarative language for telling forwarding-plane devices (switches, NICs, firewalls, filters, etc) how to process packets.

### How does P4 relate to SDN and OpenFlow?
P4 lets you tell the switch how to process packets and auto-generates an API connecting the control plane and the forwarding plane. So, a P4 program could configure a switch to look like an OpenFlow 1.0 switch, or an OpenFlow 1.4 switch, or something else entirely. And the control plane that populates the match-action rules at runtime could be centralized or distributed, running on a separate controller or directly on the switch. The main point of P4 is to free the network from low-level, proprietary interfaces to fixed-function chip-sets, to enable protocol-independent packet processing across a range of target platforms.
[...Read more on p4](https://github.com/joe019/SDN-Simulation/wiki/3.-P4)

## References      
[Developer FAQs](https://www.nsnam.org/wiki/Developer_FAQ)<br>
[User FAQs](https://www.nsnam.org/wiki/User_FAQ)<br>
[OpenFlow Switch Device](https://www.nsnam.org/doxygen/group__openflow.html)<br>
[OpenFlow switch support](https://www.nsnam.org/docs/release/3.29/models/html/openflow-switch.html)
