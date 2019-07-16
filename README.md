
# Kubernetes Cluster w Intel Movidius VPU, Raspberry PIs, Load Balancer and more

Thanks to [TechSmith](https://www.techsmith.com/) for providing free tools so I can screenshot, capture windows and recorded videos that are used in this public post.

## Objectives  
There exists many blog posts on building Kubernetes cluster with RaspberryPis. Idea behind this project is not to repeat the same but  
- Focusing on Machine Learning, build an inference cluster that can distribute requests on the worker nodes of the cluster.
- Building a cluster with a Load Balancer to distribute the work with different load balancing methodologies. Do Load balancing in software layer with a dedicated node or on hardware layer with edge router.
- Use VPU hardware acceleration to speed up the inference process. Utilize Intel OpenVino with Movidius VPU on RaspberryPi
- Visualize & Monitor: end to end network traffic, load balancing, K8s cluster status
- Monitor per Raspberry Pi power consumption.

With this project I create a 7 node Raspberry Pi cluster (1 master, 6 workers) where each worker node has a very cheap, low power consuming, powerful neural network engine - Intel VPU. When I purchased my first VPU it was ~70$ including the tax but price goes up and down and not sure the cost per USB based VPU now. What you get for 70$ is amazing and wanted to implement my own inference cluster with possible lowest cost. 


## 1. BOM - Bill of Materials
You may not need exact same items like i.e. Unifi Switch with PoE support, or you may skip the USG Security gateway. I ordered these items and models by considering the future projects, or some items are already coming from previous projects. Some of these items are just to deep dive in network traffic managements etc. and all these can be simplied with a cheaper solution.

6 x [Intel NCSM2485.DK Movidius Neural Compute Stick 2 with Myriad X Vision Processing Unit](https://www.amazon.com/gp/product/B07KT6361R)  
7 x [Element14 Raspberry Pi 3 B+ Motherboard](https://www.amazon.com/gp/product/B07BDR5PDW)  
7 x [Samsung 128GB 100MB/s (U3) MicroSD EVO Select Memory Card](https://www.amazon.com/gp/product/B06XWZWYVP)  
1 x [Ubiquiti UniFi Switch 8 60W (US-8-60W)](https://www.amazon.com/gp/product/B01MU3WUX1)  
1 x [Ubiquiti UniFi Switch 8](https://www.amazon.com/Ubiquiti-Networks-US-8-Unifi-Switch/dp/B01MZ32B1B/)  
1 x [Ubiquiti Unifi Security Gateway (USG)](https://www.amazon.com/gp/product/B00LV8YZLK)  
2 x [Vanbon 60W 8-Port USB Wall Charger, Multi Port USB Charger Charging Station](https://www.amazon.com/gp/product/B07C675TKM)  
1 x [SD Card Reader/Writer](https://www.amazon.com/gp/product/B006T9B6R2)

Various cables (usb, ethernet, hdmi etc.) and cable adaptors

Initial setup, how I started:  
<img src="doc/img001.jpg" width="400"/>
<img src="doc/img002.jpg" width="400"/>  
<img src="doc/img003.jpg" width="400"/>
<img src="doc/img004.jpg" width="400"/>  
<img src="doc/img005.jpg" width="400"/>

Final setup, how I ended up with:  
<img src="doc/img006.jpg" width="400"/>
<img src="doc/img007.jpg" width="400"/>  
<img src="doc/img008.jpg" width="400"/>
<img src="doc/img009.jpg" width="400"/>  
<img src="doc/img010.jpg" width="400"/>
<img src="doc/img011.jpg" width="400"/>  
<img src="doc/img012.jpg" width="400"/>
<img src="doc/img013.jpg" width="400"/>  
<img src="doc/img014.jpg" width="400"/>
<img src="doc/img015.jpg" width="400"/>  
<img src="doc/img016.jpg" width="400"/>
<img src="doc/img017.jpg" width="400"/>  



## 2. Setup
### 2.1 Network devices and connections
Newtwork components:  
- Home Router
- USG
- Switch 1 - S1
- Switch 2 - S2  

Connection setup:
- Connect your Internet cable (from your home router etc.) to WAN1 port of USG (Unifi Security Gateway).
- Connect your Laptop's ethernet port to LAN1 port of USG
- Connect S1 port 8 to the LAN2 port of USG
- Connect S1's port 1-6 to your 6 of the Raspberry Pi's Ethernet ports respectively  
- Connect S1 port 7 to S2's port 8
- Connect remaining Raspberry Pi's to S2

  ![](doc/img01.png)

### 2.2 Local machine
Local machine is the one refered to laptop in the above diagram. It is the machine to be used for monitoring network, monitoring K8s cluster state, Manage K8s cluster, installing raspberry pi images on SD cards, terminal connection to K8s and load balancer etc. (for this project I used a laptop with Bash shell available)

Why I put a seperate section on this: I face with minor issues that took some time to solve. They wre mainly related with my laptop's settings so, be sure to:  
- Disable the firewall on your laptop (by default windows, mac, unix have some built-in firewalls enabled.)
- Disable VPN like connections (if you have VPN like services enabled by defaut, dont forget to disable them.)
- and any other layer that may prevent to freely access to any node shown in the above architecture.

Sure above settings will make your laptop open for any external vulnerabilities. you may forget to re-enable them and may connect to public network in somewhere else in a coffee shop etc. so be careful to progressively enable them again and only open just required ports once you have a working K8s project mentioned below.

In this project I am using UniFi switch, router and gateway. To monitor all these network devices and configure them, I use UniFi application that comes along with these devices and install them on my laptop. 

- Once the UniFi app installed, launch it. It will behave like a web server on your laptop and access to it though the web interface:
                Launch Browser - Type http://localhost:8443

- Disable firewall on the USG (at least for now, until we are sure the K8s project up and running)  
Port IPs:  
  - Wan1: set to 192.168.1.1  
  - Lan1: set to 192.168.1.1  
  - Lan2: set to 192.168.2.1  

- K8s nodes (Raspberry PIs) will be attached on a switch that is under Lan2 port of the USG. Here is the list of IP addresses host names that I planned to assign to each RPi (names refered to the above architecture diagram) 

  | ID          | IP           | Hostname  |
  | ----------- | ------------ | --------- |
  | RPi 0       | 192.168.2.20 | k8s-00    |
  | RPi 1       | 192.168.2.21 | k8s-01    |
  | RPi 2       | 192.168.2.22 | k8s-02    |
  | RPi 3       | 192.168.2.23 | k8s-03    |
  | RPi 4       | 192.168.2.24 | k8s-04    |
  | RPi 5       | 192.168.2.25 | k8s-05    |
  | RPi 6       | 192.168.2.26 | k8s-06    |
  | RPi 7       | 192.168.2.27 | k8s-07    |

### 2.3 SSH Client & Terminal Window
Since we are dealing with multiple machines (K8s nodes) and using same commands almost on all nodes, I prefer to use TMux and ITerm which gives lots of flexibility to open SSH sessions in parallel to multiple nodes and sends same commond to all. TMux keep session open even you close the terminal window.

Here is the page that I refer to handle "how to send single command to multiple terminals" issue:
https://coderwall.com/p/3uq7gw/ssh-to-many-hosts-in-split-panes-with-iterm  


Referring to above IP table, IPs to be assigned to RPi nodes, created following script to be used with iTerm terminal client. Script is named as *K8s.scpt*  
  
  ![](doc/img02.jpg)

From iTerm menus, if you click on K8s.scpt, it will open 8 ssh window per nodes. It will look like as below once you have the RPi nodes up and running:

  ![](doc/img03.png)

Here is the contents of the script file *K8s.scpt*
```
set hostnames to {"pi@192.168.2.20", "pi@192.168.2.21", "pi@192.168.2.22", "pi@192.168.2.23", "pi@192.168.2.24", "pi@192.168.2.25", "pi@192.168.2.26", "pi@192.168.2.27"}

if application "iTerm" is running then
	tell application "iTerm"
		create window with default profile
		tell current tab of current window
			select
			tell current session
				
				-- make the window fullscreen
				tell application "System Events" to key code 36 using command down
				split horizontally with default profile
				
				set num_hosts to count of hostnames
				repeat with n from 1 to num_hosts
					if n - 1 is (round (num_hosts / 2)) then
						-- move to lower split
						tell application "System Events" to keystroke "]" using command down
					else if n > 1 then
						-- split vertically
						tell application "System Events" to keystroke "d" using command down
					end if
					delay 1
					write text "ssh " & (item n of hostnames)
				end repeat
			end tell
		end tell
	end tell
else
	activate application "iTerm"
	
end if
```

Once the terminal windows to all nodes open, you can press **Shift + Command + I** key combinations to send keypresses to all windows (whenever we setup the nodes in the upcoming sections).

### 2.4 RaspberryPi (RPi) Nodes
On this stage of the project we will burn the Raspbian OS image into SD cards and install them on the RPi cards.  

One approach to setup each raspberry pi node is to burn raspbian OS on the SD card and plug it into RPi cards. Plug an HDMI monitor and keyboard per RPi card, one by one to setup its network settings. Repat it for 8 or more RPi. Since I dont want to plug keyboard and monitor each time to the RPi card, I do it another way. I just copy an empty ssh file into the OS image on the SSD card (immediately after I burn the OS image to SD card at my laptop). Once powered up, RPi will automatically get IP address from DHCP server, and connected to the network. Since we created an empty ssh file under /boot folder, it will also allow us to open ssh terminal window to this RPi. By this way you eliminate monitor and keyboard connection per RPi card and handle all just from single computer, your laptop.

BOM:
- Download [Raspbian Stretch Lite](https://downloads.raspberrypi.org/raspbian_lite_latest) image
- Use [Micro SD card adaptor](https://www.amazon.com/gp/product/B006T9B6R2) to burn the raspbian stretch lite image onto 8 SD card.  

Burn the Raspbian OS image to the Micro SD cards. Once the images burned, SD cards may be automaticall un-mounted from the laptop. If that is the case, re-mount each SD card. In your favorite file explorer window, you will see the SD card's contents, which is a folder named Boot. Drag and drop an empty file which is named "ssh" into this folder. Be sure that the filename is as is "ssh" without any file extension like "ssh.txt"

### 2.5 Raspbian OS settings
It is like using Ubuntu, not much difference. Applied following changes on the RPi nodes.

- Power up RPi nodes one after the other once you finish the per node setup below (dont power up all at the same time). This is mainly because each node will automatically get an IP address from the DHCP server and it may be difficult to differentiate which node has which IP address. Actually this is very simple to learn from UniFi App which shows the IP address assigned per network switch port but assuming not everybody using this type of managed switch.

Here is the command you should use in your iTerm app on your laptop to open SSH terminal to the corresponding RPi device. (sure you may prefer another terminal client app)

```
> ssh pi@IP_ADDRESS_OF_THE_RASPBERRY_PI
```

#### 2.5.1 Change password
By default your raspberery pi has a default username and password:  
    *username: pi*  
    *password: raspbian*  

From your Laptop open an SSH session to your raspbery pi. You know the IP address of each node as mentioned above. Change the default password to a more simple one, lets say "pi". So username and password are same. (sure this is not a good practice but for this development environment, you will test/debug hundred times, you may not want to type a complex password each time)

use following commands to change the password of the current user (which is user "pi") and its group.
```
sudo passwd pi

sudo usermod -aG sudo pi
```

#### 2.5.2 Hostname & IP Address
Open SSH terminal to RPi. create a file by using following command:

```
nano hostname_and_ip.sh
```

it will open a simple file content editor, create a file named hostname_and_ip.sh You can write anything using this editor and save in to the file.


Lets copy/paste the following content into the file named hostname_and_ip.sh

```
#!/bin/sh
hostname=$1
ip=$2 # should be of format: 192.168.1.100
dns=$3 # should be of format: 192.168.1.1

# Change the hostname
sudo hostnamectl --transient set-hostname $hostname
sudo hostnamectl --static set-hostname $hostname
sudo hostnamectl --pretty set-hostname $hostname
sudo sed -i s/raspberrypi/$hostname/g /etc/hosts

# Set the static ip
sudo cat <<EOT >> /etc/dhcpcd.conf
interface eth0
static ip_address=$ip/24
static routers=$dns
static domain_name_servers=$dns
EOT
```

Press CTRL+X key combination to save and close the window (see instructions on the screen to see how to save and exit).

We created a file, shell script, that will help us to set a static IP and a hostname to our RPi. By this way when we reboot the RPi, we will be sure that it will have same IP each time.

just type the following command on the terminal window on to your RPi device:
```
sh hostname_and_ip.sh k8s-00     192.168.2.20    192.168.2.1  
```
it will call the shell script we created above and will set network IP, hostname and gateway. First parameter "k8s-00" is the host name (referring to above table where we wrote the proposed IP addresses and hostnames that we will assign to the RPi devices). Second parameter is the IP address and the third one is the gateway (see above network architecture. Since we connect all RPi devices to the LAN2 port of the UniFI USG which we set 192.168.2.1 as its IP)

Reapeat this steps per RPi device (rest of the other 7 RPI nodes) by using following lines per RPi nodes.
```
sh hostname_and_ip.sh k8s-01     192.168.2.21    192.168.2.1  
sh hostname_and_ip.sh k8s-02     192.168.2.22    192.168.2.1  
sh hostname_and_ip.sh k8s-03     192.168.2.23    192.168.2.1  
sh hostname_and_ip.sh k8s-04     192.168.2.24    192.168.2.1  
sh hostname_and_ip.sh k8s-05     192.168.2.25    192.168.2.1  
sh hostname_and_ip.sh k8s-06     192.168.2.26    192.168.2.1  
sh hostname_and_ip.sh k8s-07     192.168.2.27    192.168.2.1  
```

#### 2.5.3 Localization, Keyboard layout, timezone settings
One you set the IP addresses of each RPi device, you dont need to repeat same commands on each RPi node seperately. Now we can use the iTerm client with the script "K8s.scpt" that we wrote above. Referring to section 2.3, open 8 sub SSH terminal window under a single main window to all 8 RPi devices. 

Press **Shift + Command + I** key combination to enable multi-window command send feature. Whatever you type will be sent to all 8 windows.

type following command in one of the windows and make appropriate changes to your RPi nodes through a user interface.
```
sudo raspi-config
```

change keyboard layout to make same as your host device, laptop. Change your location and time zone. And any other setting that you want.

Reboot all the RPi devices to have all these changes reflected
```
sudo reboot
```

#### 2.5.4 Software Update
All your RPi devices are up and running with static IP, all configurations completed. Now lets update the Raspbian OS and packages.

Type following commands on all RPi devices (now you know how to do it in an easy way over all devices with iTerm client.)

```
// Update
sudo apt-get update && sudo apt-get -y upgrade

// install git & pip
sudo apt-get -y --no-install-recommends install \
        git \
        python3-pip

```

#### 2.5.4  Disable the Pi3's WLAN & Bluetooth
We sourced the power to our RPi devices from [Vanbon 60W 8-Port USB Charging Station](https://www.amazon.com/gp/product/B07C675TKM) mentioned in the BOM above. I prefered this device since it shows detailed power consumption per USB port. So I can monitor how much power each RPi is consuming. I noticed that WIFI and Bluetooth enabled RPi device consumes a little bit more power. Also because I dont need any more radio frequency close to me, I disable these two feature: WIFI and Bluetooth.

To disable WIFI and Bluetooth, edit the "/boot/config.txt" file with favorite text editor nano (I dont want to make this post more complicated by using Vi)

```
sudo nano /boot/config.txt
```

Now add the below two lines at the end of the /boot/config.txt file (https://www.raspberrypi.org/forums/viewtopic.php?t=138610)

```
dtoverlay=pi3-disable-wifi
dtoverlay=pi3-disable-bt
```

Reboot to have changes reflected
```
sudo reboot
```

## 3. Install Docker
Lets install the Docker engine onto all nodes (including master and worker nodes)

Follow the below command sequence on all RPi devices to have Docker engine installed.

```
# Install the following prerequisites.
sudo apt-get install apt-transport-https ca-certificates software-properties-common -y

# Download and install Docker.
curl -fsSL get.docker.com -o get-docker.sh && sh get-docker.sh

# Give the ‘pi’ user the ability to run Docker.
sudo usermod -aG docker pi

# Import Docker CPG key.
sudo curl https://download.docker.com/linux/raspbian/gpg | sudo apt-key add -
```

With below commands, first we edit /etc/apt/sources.list file and add some lines.

```
# Setup the Docker Repo.
sudo nano /etc/apt/sources.list

# Add the following line and save
deb https://download.docker.com/linux/raspbian/ stretch stable
```

With below lines, update the system software, packages etc. start docker engine as OS level service and check the docker version to verify succesful installation.

```
sudo apt-get update && sudo apt-get -y upgrade

# Start the Docker service.
sudo systemctl start docker.service

# To verify that Docker is installed and running.
docker info

docker --version
//docker run armhf/hello-world
```

## 4. Install Kubernetes (K8s)
!!! Be carefull!!! 
- not all commands will run on both master and worker nodes. Some of the below commands to be given only on master or worker nodes.
-  RPi 0 (IP address = 192.168.2.20) is not part of the K8s cluster we are building. It have seperate installation section below and this node will be used just for load balancing (also you may use it for testing the inference engine.)  So when we say master and worker nodes (all K8s nodes), they are only RPi 1 to RPi 7, where RPi 1 is the master and others are the workers.

### 4.1 Disable SWAP space
Run below commands on all K8s nodes (master and worker ones)

```
# Disable Swap (below is a multiline command seperated by \ character. Copy/Paste below three lines all together in the SSH window)
sudo dphys-swapfile swapoff && \
sudo dphys-swapfile uninstall && \
sudo update-rc.d dphys-swapfile remove

# Ref: https://raspberrypi.stackexchange.com/questions/84390/how-to-permanently-disable-swap-on-raspbian-stretch-lite
# Edit /etc/dphys-swapfile
sudo nano /etc/dphys-swapfile
# make following change in the appropriate line. Set Swap size to 0
CONF_SWAPSIZE=0

# Follwing should return nothing for successful swap off
sudo swapon --summary

# ref: https://downey.io/blog/exploring-cgroups-raspberry-pi/
sudo cp /boot/cmdline.txt /boot/cmdline_backup.txt
orig="$(head -n1 /boot/cmdline.txt) cgroup_enable=cpuset cgroup_memory=1 cgroup_enable=memory"
echo $orig | sudo tee /boot/cmdline.txt
```

Rebooth all the nodes in the cluster
```
sudo reboot
```

### 4.2 Install kubeadm
Run below commands on all K8s nodes
```
# Add repo list and install kubeadm
curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add - && \
echo "deb http://apt.kubernetes.io/ kubernetes-xenial main" | sudo tee /etc/apt/sources.list.d/kubernetes.list && \
sudo apt-get update && \
sudo apt-get install -y kubeadm
```

### 4.3 Master Node Setup
Run following commands only on Master node

```
# Pre-pull images
#   k8s.gcr.io/kube-apiserver:v1.14.0
#   Pulled k8s.gcr.io/kube-controller-manager:v1.14.0
#   Pulled k8s.gcr.io/kube-scheduler:v1.14.0
#   Pulled k8s.gcr.io/kube-proxy:v1.14.0
#   Pulled k8s.gcr.io/pause:3.1
sudo kubeadm config images pull

# Install Weave Net as a network overlay
sudo kubeadm init --token-ttl=0
```


After several minutes, it will end up with following lines on the terminal window.

```
Your Kubernetes control-plane has initialized successfully!

To start using your cluster, you need to run the following as a regular user:

  mkdir -p $HOME/.kube
  sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
  sudo chown $(id -u):$(id -g) $HOME/.kube/config

You should now deploy a pod network to the cluster.
Run "kubectl apply -f [podnetwork].yaml" with one of the options listed at:
  https://kubernetes.io/docs/concepts/cluster-administration/addons/

Then you can join any number of worker nodes by running the following on each as root:

kubeadm join 192.168.2.21:6443 --token z1j708.6wmgwduiunngkjmu \
    --discovery-token-ca-cert-hash sha256:3981bb0e7082826e19af99e552a59bb1562aafe76eece50a92fd28a07f0d28d5
```
Above lines are very important, you should note them (what I am doing is, each time I need to run above command I copy/paste them into this text window)

somethimes it gives error and you DO NOT get the above output. I re-run the command with following parameters:

```
sudo kubeadm init --token-ttl=0 --ignore-preflight-errors=all
```

As next step, as mentioned in the above success output, we run the following command:

```
rm -rf $HOME/.kube
mkdir -p $HOME/.kube
sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config
```

Lets not yet run the "kubeadm join 192.168.2.21:6443 --token z1j708.6wmgwduiunngkjm.... " command. We take note about above success lines and will come back to them later. We are no still on the master node.
        
### 4.4 Install the Weave Net network driver
Install network drivers to let K8s nodes communications.

Still running on the master node.

```
kubectl apply -f "https://cloud.weave.works/k8s/net?k8s-version=$(kubectl version | base64 | tr -d '\n')"

// Check Weave status and logs
//  sudo curl -L git.io/weave -o /usr/local/bin/weave
//  sudo chmod a+x /usr/local/bin/weave
//  weave status
//  kubectl get pods -n kube-system -l name=weave-net -o wide

// kubectl get pods -n kube-system -o wide | grep weave-net
//  kubectl logs weave-net-cntqz -n kube-system weave-npc
//  kubectl describe  daemonset -n kube-system kube-proxy
```

### 4.5 Check if all K8s services up and running

Reboot all nodes (master and workers)
```
sudo reboot
```


Run following command just on master node
```
# Wait for few minutes to have system up and running
# Check if everything is running without any issue
kubectl get all --all-namespaces
```

Result of this command should look like someting:
```
NAMESPACE     NAME                                        READY   STATUS    RESTARTS   AGE
kube-system   pod/coredns-fb8b8dccf-hxmrf                 1/1     Running   0          18m
kube-system   pod/coredns-fb8b8dccf-mkcpt                 1/1     Running   0          18m
kube-system   pod/etcd-k8s-worker-06                      1/1     Running   0          18m
kube-system   pod/kube-apiserver-k8s-worker-06            1/1     Running   0          18m
kube-system   pod/kube-controller-manager-k8s-worker-06   1/1     Running   0          18m
kube-system   pod/kube-proxy-5zxlr                        1/1     Running   0          18m
kube-system   pod/kube-scheduler-k8s-worker-06            1/1     Running   0          18m
kube-system   pod/weave-net-qsdsh                         2/2     Running   0          10m

NAMESPACE     NAME                 TYPE        CLUSTER-IP   EXTERNAL-IP   PORT(S)                  AGE
default       service/kubernetes   ClusterIP   10.96.0.1    <none>        443/TCP                  18m
kube-system   service/kube-dns     ClusterIP   10.96.0.10   <none>        53/UDP,53/TCP,9153/TCP   18m

NAMESPACE     NAME                        DESIRED   CURRENT   READY   UP-TO-DATE   AVAILABLE   NODE SELECTOR   AGE
kube-system   daemonset.apps/kube-proxy   1         1         1       1            1           <none>          18m
kube-system   daemonset.apps/weave-net    1         1         1       1            1           <none>          10m

NAMESPACE     NAME                      READY   UP-TO-DATE   AVAILABLE   AGE
kube-system   deployment.apps/coredns   2/2     2            2           18m

NAMESPACE     NAME                                DESIRED   CURRENT   READY   AGE
kube-system   replicaset.apps/coredns-fb8b8dccf   2         2         2       18m
```

If some of the services have "0" in their "Ready" state, wait to have them ready. This may take several minutes. 

Run the following command on all nodes (master + workers) (https://github.com/kubernetes/kubeadm/issues/312)
```
sudo sysctl net.bridge.bridge-nf-call-iptables=1
```

### 4.5 Register Worker nodes into Master node

Run following command just on worker nodes. This command is the one that we noted above. It should contain different security keys in your case. Dont forget to run it in "sudo" mode, as the prefix implies.

```
sudo kubeadm join 192.168.2.21:6443 --token z1j708.6wmgwduiunngkjmu \
          --discovery-token-ca-cert-hash sha256:3981bb0e7082826e19af99e552a59bb1562aafe76eece50a92fd28a07f0d28d5
```

### 4.6 Check the status of the K8s nodes statuses
Run the following commands on the master node to see if everything is running as expected. After registering worker nodes it may again take some time to have them in ready state.

```
pi@k8s-01:~ $ kubectl get nodes
```
output should look like:
```
NAME     STATUS   ROLES    AGE     VERSION
k8s-01   Ready    master   2d17h   v1.14.2
k8s-02   Ready    <none>   2d16h   v1.14.1
k8s-03   Ready    <none>   2d16h   v1.14.1
k8s-04   Ready    <none>   2d16h   v1.14.1
k8s-05   Ready    <none>   2d16h   v1.14.1
k8s-06   Ready    <none>   2d16h   v1.14.1
k8s-07   Ready    <none>   2d16h   v1.14.1
```

## 5. Install Kubernetes (K8s) Dashboard
We need to seperately install the dashboard on the master node to monitor the K8s nodes and manage the cluster.

### 5.1 Install Dashboard
Be sure getting the ARM-HEAD version (latest dev! version for ARM)

Run following commands on master node:

```
kubectl apply -f https://raw.githubusercontent.com/kubernetes/dashboard/master/aio/deploy/recommended/kubernetes-dashboard-arm-head.yaml
```

Also run all the below lines as a single command:

```
cat <<EOF | kubectl apply -f -
apiVersion: v1
kind: ServiceAccount
metadata:
  name: admin-user
  namespace: kube-system
---
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRoleBinding
metadata:
  name: admin-user
roleRef:
  apiGroup: rbac.authorization.k8s.io
  kind: ClusterRole
  name: cluster-admin
subjects:
- kind: ServiceAccount
  name: admin-user
  namespace: kube-system
---
apiVersion: rbac.authorization.k8s.io/v1beta1
kind: ClusterRoleBinding
metadata:
  name: kubernetes-dashboard-head
  labels:
    k8s-app: kubernetes-dashboard-head
roleRef:
  apiGroup: rbac.authorization.k8s.io
  kind: ClusterRole
  name: cluster-admin
subjects:
- kind: ServiceAccount
  name: kubernetes-dashboard-head
  namespace: kube-system
EOF
```
you can run the following command on the master node to see if the dashboard services are running fine:
```
kubectl describe service -n kube-system kubernetes-dashboard
```

### 5.2 Access K8s Dashboard from Laptop
To be able to access the master node and the dashboard we need the first install Kubectl on the laptop. Here is the installation instruction for "kubectl" on laptop: https://kubernetes.io/docs/tasks/tools/install-kubectl/ 

After kubectl installation on the laptop, now copy the existing kube config files from the master node into laptop.

Run following command on the host machine, laptop.
```
cd ~
rm -rf .kube
scp -r pi@192.168.2.21:/home/pi/.kube .
```

#### 5.2.1 Launch the dashboad UI on a browser
On the laptop, terminal window, run following command:
```
kubectl proxy
```
this will run the proxy server on the laptop and communicates with your K8s's master node.  

Now on your laptop, open a browser window and goto the following URL address:
```
http://127.0.0.1:8001/api/v1/namespaces/kube-system/services/https:kubernetes-dashboard-head:/proxy/
```

Landing page should look like this:  
  ![](doc/img04.png)

You need a token to enter the dashboard. Type the following command in the master node to get the token:  
```
kubectl -n kube-system describe secret $(kubectl -n kube-system get secret | grep admin-user | awk '{print $1}')
```
  ![](doc/img05.png)

Copy/Paste the token (sure excluding the "token: " attribute) into the browser to Sign In to the dashboard.  

**!!! Important !!!**
If you dont interact with the dashboard for several minutes, your session will time out and you will see continous progress circles. In such case just logout from the dashboard by using the menu item on top right corner of the dashboard window and re-login.


With this latest step we have the K8s cluster up and running.


## 6. Develop Inference Client & Server applications

### 6.1 gRPC & Protobuf for C++

```
cd $HOME && \
        git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc && \
	cd $HOME/grpc && \
        git submodule update --init && \
	make && \
	make install

  cd $HOME/grpc/third_party/protobuf && \
        make && \
	make install
```



Now we develop our client and server applications in C++. Client application takes two parameters as input:  
- Server address
- Path to the image file to be inferred

Our server application doesnt take any address but waits to receive an inference request. Once request received, which is an image file sent to server, it will infer the image using OpenVino framework to access the Intel Neural Compute Stick to execute the inference with pre-defined face detection model. Once the inference done, result is sent as a message in JSON format.

[inference_client.cc](./iovis/client/c++/inference_client.cc)  
[inference_server.cc](./iovis/server/c++/inference_server.cc)

Moreover, one another capability that I implemented and wanted to test the performance is the gRPC message protocol. All messages exchanged between client and server are over gRPC.

Simply you need to run the **make** file in the cleint and server folders to compile the applications and have them ready to run.

I also created a very basic HelloWorld type gRPC client/server app within this Github repo which you may start with for debugging purposes (I did in my case to see if K8s cluster works fine, load balancer and network connections fine etc.)

### Machine Learning Model in Server Application:
Even we set the ML model statically in the code, it is easy to replace the ml model path with another one that you want. In this project I used **face-detection-adas-0001** model from Intel [OpenVino samples](https://docs.openvinotoolkit.org/latest/_face_detection_adas_0001_description_face_detection_adas_0001.html.)

This model is "Face Detection (MobileNet w reduced channels + SSD w weights sharing)". You may find many other interesting pre-trained ML models under [OpenCV - Open Model Zoo](https://github.com/opencv/open_model_zoo/blob/master/intel_models/index.md)

Regarding face detection, there is an [enhanced version](https://docs.openvinotoolkit.org/latest/_face_detection_retail_0004_description_face_detection_retail_0004.html) of this model also under OpenVino samples. 


## 7. Deploy the app to K8s
Having the clone of this GitHub repository on the master node of your K8s cluster, run the following commands on the master node. In our case, if you followed the above same steps to create your K8s cluster, master node name should be "k8s-01" which has the IP address 192.168.2.21

### 7.1 Create Docker Image
Create a docker file that will define the container image content (this is already created for you in the GitHub repo). Since this container will run on a RaspberryPI device, we will put the server application along with all required runtimes on top of a light raspbian OS.  

nano Dockerfile
```
   Dockerfile content
```
Here is the docker file pre-created for you (unless you want to modify it with your development purpose):  
[Dockerfile](./iovis/Dockerfile)


Use the following command to build the image. Change "mkasap/iovis:v7" with any other tag that you want. Here "mkasap" needs to be same as your www.docker.io handle. Rest is just image name and a version number that you can assign

```
docker build -t "mkasap/iovis:v7" .
```

Using docker.io is optional. You may compile your docker container image on Master node and keep it there... But once you format the master node, or want to clone another cluster etc. you need to re-compile it. So in my case, after creating the container image, I push it into the cloud, docker.io and pulled down anytime I need it. Since the container size is ~3GB, dont worry about push and pull each time. If the container image already exist in your K8s cluster system automatically use the local version and will not download again and again unless any change in any layers of the container.

With the following code you can run the docker container with the image that you built
```
docker container run --privileged --rm --net=host -v /dev:/dev -i -t mkasap/iovis:v7 /bin/bash
```
Here because we are accessing a USB device on the host machine, I used the --privileged flag to access any resource and mapped the host /dev folder into the container /dev folder. Also set the --net=host parameter so can access the host network. All these three settings are required to be able to acccess the Intel NCS2 within the container.

### 7.2 Push/Pull Docker image to docker.io
As said above, this is optional.

Once you build the docker image locally, you can push it into docker.io so it can be accessed, pulled from anywhere with Internet access. To push the docker image first, you must have an account at docker.io, second you need to login this account in your local computer to be able to access it (either push or pull images)

To push the image I used the following commands:
```
docker login docker.io

# Below commang will take some time depending on network speed... uploading ~3GB data
docker push mkasap/iovis:v7
```

To pull the same image from anywhere you can use the following commands. We will not need it for now because our K8s cluster will automatically pull the image and deploy the containers. We will only need to specify docker.io credentials in K8s deployment which we will cover in the next sections.
```
docker login docker.io
docker pull mkasap/iovis:v7
```

Now we have container image ready in the cloud at docker.io.

### 7.3 Set cluster node labels
Since each worker node has single Intel NCS2, only one model will be running on a single worker node. Also we want to be sure that our inference server will be running on only the worker nodes which has Intel NCS2 attached. To guarantee this, we set labels on the worker nodes that has Intel NCS2 attached. This worker node labeling might seem redundent but later if we add more worker nodes like some other compute tarkets such as FPGA, GPU etc. we can easily differentiate which server will work on which compute tarket. i.e. I made [Nvidia nano](https://developer.nvidia.com/embedded/learn/get-started-jetson-nano-devkit) one another worker node to send some other type of data (hope to cover it in an extension of this post)

get list of nodes with their names
```
kubectl get nodes
```

out of these nodes, only the ones named k8s-02, k8s-03, k8s-04, k8s-05, k8s-06, k8s-07 have Intel NCS2 attached. So setting same label on all these workers.

``` 
kubectl label nodes k8s-02 k8s-03 k8s-04 k8s-05 k8s-06 k8s-07 nodetype=iov
```

With the following we check if these nodes have the labels correctly assigned
```
kubectl get nodes --show-labels
```

### 7.4 Deploy inference server on K8s cluster
Since we will be pulling docker images from the docker.io which is (at least for this project) in a private repo and requires authentication, I will set user credentials for K8s engine to let it pull container images without any error.

```
kubectl create secret docker-registry regcred --docker-server=docker.io --docker-username=mkasap --docker-password=YOUR_PASSWORD --docker-email=m@mustafakasap.com --namespace=default
```

you can export or view the secret with the following code:
```
kubectl get secret regcred --output=yaml
```

We will use the following multi-line command to deploy our containers in the cluster with 6 replicas (since we have only 6 worker with 6 Intel NCS2 attached). You can dump the following lines into a file and execute it by "kubectl apply -f filename" command but found below method more simple.

```
cat <<EOF | kubectl apply -f -
apiVersion: extensions/v1beta1
kind: Deployment
metadata:
  name: inferenceserver
spec:
  replicas: 6
  template:
    metadata:
      labels:
        app: inferenceserver
    spec:
      hostNetwork: true
      containers:
        - image: mkasap/iovis:v7
          securityContext:
            privileged: true 
          imagePullPolicy: Always
          name: inferenceserver
          ports:
            - name: inferenceport
              containerPort: 50051
          volumeMounts:
          - name: myriad
            mountPath: /dev
      volumes:
      - name: myriad
        hostPath:
          path: /dev
      imagePullSecrets:
        - name: regcred
      nodeSelector:
        nodetype: iov        
EOF
```

From the K8s Dashboard you can see the deployment and it may take some time in case the container image is pulled from the docker.io and not from the local copy.

<img src="doc/img018.jpg" width="800"/>  

Once deployment done, you will see all green :)  

<img src="doc/img019.jpg" width="800"/>  




## 8. Access Inference server
I had one free raspberryPi without Intel NCS installed and connected to Lan2 port of the Unifi USG gateway device as I wanted to test from another network. In your case you can simply do below debug test from master node or from another RPi device in the same lan etc.

I connected to my RPi device that I used for debugging purpose. Cloned the client application and compiled it. Here is my test command and its output:

```
pi@k8s-00:~ $ cd iovis/client/c++/
pi@k8s-00:~/iovis/client/c++ $ ./inference_client 192.168.2.22 50051 ../../test/data/sample02.jpg
Calling inference server:
	Full file name: ../../test/data/sample02.jpg
	Server address: 192.168.2.22:50051

Sending file for inference...
Inference result :
[
	{
		"detections": {
			"rectangle": [
				{
				"label": 1,
				"probability": 1,
				"width": 275,
				"height": 790,
				"left": 54,
				"top": 66
				},

...
...

				{
				"label": 1,
				"probability": 0.549805,
				"width": 1078,
				"height": 717,
				"left": 52,
				"top": 56
				}
			]
		},
	"time": 118.164
	}
]
```

When we deployed our container image into K8s worker nodes, we used the  

      hostNetwork: true

parameter in deployment script. Same parameter in native docker engine was:

      --net=host

This command quaranteed that we can access worker nodes with their host IP address which are  

      192.168.2.22 - 192.168.2.27

For this test we use the first worker node but you can test all.


## 9. Using Load Balancer to distribute the inference requests 
There are many different ways to create the load balancer. One of them is to use native K8s LoadBalancer features. Another is using bare-metal LoadBalancer like Unifi Edge Router (which I will cover in an extension to this post, hope to do). Here I will show the simplest option which is proxy load balancer.


I used a seperate RPi device as Load balancer. This device is not member of K8s cluster and in the same LAN. I used [HAProxy](http://www.haproxy.org/) for this purpose which is simple to install and manage. Also the major reason was that HAProxy provides end-to-end proxying of HTTP/2 traffic, so observe gRPC traffic over HTTP/2.

https://www.haproxy.com/blog/haproxy-1-9-2-adds-grpc-support/

<img src="doc/img021.png" width="400  "/>  

Installation of the load balancer and its configuration is very simple. Used following command to install it:

```
// get latest dev release (we need release version > 1.9 for grpc HTTP/2 support)
wget http://www.haproxy.org/download/2.0/src/haproxy-2.0.0.tar.gz

// extract the files
tar xvzf haproxy-2.0.0.tar.gz

cd haproxy-2.0.0/

make TARGET=linux-glibc

sudo make install

sudo reboot

// Finally check the version
haproxy -vv

systemctl status haproxy.service
```

After installation, edit its configuration file:

```
sudo nano /etc/haproxy/haproxy.cfg
```

I used the following configuration:

```
defaults
    option http-use-htx
    option logasap
    log global

frontend K8s
    bind *:50051
    mode tcp
    default_backend nodes

backend nodes
    mode tcp
    balance roundrobin
    server k8s-02 192.168.2.22:50051 check
    server k8s-03 192.168.2.23:50051 check
    server k8s-04 192.168.2.24:50051 check
    server k8s-05 192.168.2.25:50051 check
    server k8s-06 192.168.2.26:50051 check
    server k8s-07 192.168.2.27:50051 check

listen stats
    bind *:1923
    mode http
    stats enable
    stats uri /
    stats refresh 5s
```

As you can see from the configuration that I added 6 of my worker nodes with their host IP addresses in the load balancer. I used roundrobin work distribution method but feel free to test any. HAProxy web site has all details on possible configurations.

As you can see from the configuration file, this load balancer RPi device will have dashboard to view the LoadBalancer activity. You can access it with http://192.168.2.20:1923 Dont forget to change the IP address  with the RPi device that you install the LoadBalancer. In my case it is 192.168.2.20. When you access the dashboard it will ask username and password which you set in the configuration file as USERNAME:PASSWORD tuple.


After updating the configuration file, you must restart the LoadBalancer server with following commands:

```
haproxy  -f /etc/haproxy/haproxy.cfg

sudo service haproxy restart
sudo reboot
```
in my case reboot was the solution (maybe a bug etc.)  

and you can check its state with:

```
systemctl status haproxy.service
```

Your Load Balancer Dashboard will look like:

<img src="doc/img020.jpg" width="1200"/>  

Now call the inference_client app several times and see the changes on the dashboard.


## 10. Test with WIDERFACE Dataset
Test dataset compressed file size is ~2GB and download may take some time. I used following script to download the WIDERFace test dataset ([Thanks to Matt Haismann](https://www.matthuisman.nz/2019/01/download-google-drive-files-wget-curl.html)):

```
cd ~
export fileid=0B6eKvaijfFUDbW4tdGpaYjgzZkU
export filename=widerfacetest.zip

curl -L -c cookies.txt 'https://docs.google.com/uc?export=download&id='$fileid \
     | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1/p' > confirm.txt

curl -L -b cookies.txt -o $filename \
     'https://docs.google.com/uc?export=download&id='$fileid'&confirm='$(<confirm.txt)

rm -f confirm.txt cookies.txt
```

Extract the files under your home directory (or any other location)
```
unzip widerfacetest.zip
```

and check the number of test files we have:
```
cd ~/WIDER_test

find . -type f | wc -l
```
Which will return 16097 files.