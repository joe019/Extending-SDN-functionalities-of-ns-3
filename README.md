# Extending SDN functionalities of ns-3
## Course: Major Project (MP)
## Course Code: CO449 (MP-I), CO499 (MP-II)

### Website
https://joe019.github.io/Extending-SDN-functionality-in-ns-3/

### Steps to reproduce
1. ns3 installation
  Follow the steps given in https://www.nsnam.org/docs/tutorial/html/getting-started.html 
2. openflow configuration
  https://www.nsnam.org/docs/models/html/openflow-switch.html#building-ofsid
  now the basic configuration is ready
3. Applying the modified code
  * goto a new directory and clone the repository
  > git clone https://github.com/joe019/Extending-SDN-functionality-in-ns-3.git
  * goto your ns3 workspace and replace the folder "src/openflow" with the "Extending-SDN-functionality-in-ns-3/openflow/
" from the cloned repository
4. you can run the examples using the command 
 >./waf --run \"openflow-switch --verbose\"
5. any other example can be run by replacing openflow-switch with the example name


## References
