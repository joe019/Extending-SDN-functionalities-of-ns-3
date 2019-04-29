# Extending SDN functionalities of ns-3
## Course: Major Project (MP)
## Course Code: CO449 (MP-I), CO499 (MP-II)

### Team
#### ASWANTH P P (15CO112)
#### GAUTHAM M  (15CO118)
#### JOE ANTONY (15CO220)

### Website
https://joe019.github.io/Extending-SDN-functionality-in-ns-3/

### Overview
This project is an attempt to extend some SDN functionalities present in ns-3

### Steps to reproduce
1. Installing ns-3 and enabling OpenFlow integration <br>
Check out this [link](https://github.com/joe019/Extending-SDN-functionality-in-ns-3/wiki/5.-ns-3)!
2. Clone this repository
git clone https://github.com/joe019/Extending-SDN-functionality-in-ns-3.git
3. Copy the contents
Goto your ns3 workspace and replace the folder "src/openflow" with "Extending-SDN-functionality-in-ns-3/openflow/
" from the cloned repository.
4. Run an example 
./waf --run \"openflow-switch --verbose\"
Any other example can be run by replacing "openflow-switch" with the example name


## References
1. Tom Henderson, et al.(2019),"ns-3 Tutorial Release ns-3.29"
2. "Software-Defined Networking Definition" The Open Networking Foundation,
https://www.opennetworking.org/sdn-definition, 2017
3. Quentin Monnet(2016),"An introduction to SDN", WhirlOffload https://qmonnet.github.io/whirl-offload/2016/07/08/introduction-to-sdn/
4. Anders Nygren, et al.(2015), "OpenFlow Switch Specification Version 1.5.1" The Open Networking Foundation
5. Luciano Jerez Chaves, ”et al.’(2016), "Integrating OpenFlow to LTE: Some issues
toward software-defined mobile networks", 7th International Conference on New
Technologies, Mobility and Security (NTMS), Paris, France
6. Rihab Jmal and Lamia Chaari Fourati, "Implementing shortest path routing mech-
anism using Openflow POX controller" The 2014 International Symposium on
Networks, Computers and Communications, Hammamet, Tunisia
7. Pat Bosshart, ”et al.’(2014), "P4: Programming Protocol-IndependentPacket Pro-cessors" ACM SIGCOMM Computer Communication Review(9), 87-95
8. Luciano Jerez Chaves(2016), "OpenFlow 1.3 module for ns-3", ComputerNetworks Laboratory of the University of Campinas (Unicamp), Brazil,
http://www.lrc.ic.unicamp.br/ofswitch13/
[9] Luciano Jerez Chaves, ”et al.’(2016), "OFSwitch13: Enhancing ns-3 with Open-
Flow 1.3 Support" WNS3 ’16(8), 33-40
