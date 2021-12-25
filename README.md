Personal Implementation of a Virtual Machine (VM) written in C to simulate a fictional computer called LC-3. Attached are 2 simple LC-3 programs 2048 and Rogue for testing purposes.

[Tutorial](https://justinmeiners.github.io/lc3-vm/#1:15)

[LC-3 Guide](https://justinmeiners.github.io/lc3-vm/supplies/lc3-isa.pdf)

## To run code

1. `gcc lc3.c -o lc3` to compile
2. `./lc3 <path-to-program>`

## Virtual Machines

- program that acts like a computer
- simulates a CPU and a few other hardware components
- perform arithmetic, read and write to memory, and interact with I/O devices
- 1 or more virtual “guest” machines run on a physical “host” machine
- each virtual machine runs its own operating system and functions separately from the other VMs, even when they are all running on the same host

## 2 types of VMs

1. Process VM
    - allows a single process to run as an application on a host machine
    - providing a platform-independent programming environment by masking the information of the underlying hardware or operating system
    - 1 example is the Java Virtual Machine, which enables any operating system to run Java applications as if they were native to that system

2. System VM
    - fully virtualized to substitute for a physical machine
    - supports the sharing of a host computer’s physical resources between multiple virtual machines
    - each runs its own copy of the operating system
    - virtualization process relies on a hypervisor, which can run on bare hardware, such as VMware ESXi, or on top of an operating system

## 5 types of Virtualization

1. Hardware Virtualization
    - virtual versions of computers and operating systems (VMs) are created and consolidated into a single, primary, physical server
    - hypervisor communicates directly with a physical server’s disk space and CPU to manage the VMs
    - hardware virtualization, which is also known as server virtualization, allows one machine to simultaneously run different operating systems
2. Software Virtualization
    - creates a computer system complete with hardware that allows one or more guest operating systems to run on a physical host machine
    - Android OS can run on a host machine that is natively using a Microsoft Windows OS
    - applications can be virtualized and delivered from a server to an end user’s device, such as a laptop or smartphone
    - allows employees to access centrally hosted applications when working remotely
3. Storage Virtualization
    - consolidate multiple physical storage devices to appear as a single storage device
    - increased performance and speed, load balancing and reduced costs
    - helps with disaster recovery planning
4. Network Virtualization
    - multiple sub-networks can be created on the same physical network by combining equipment into a single, software-based virtual network resource
    - divides available bandwidth into multiple, independent channels, each of which can be assigned to servers and devices in real time
5. Desktop Virtualization
    - separates the desktop environment from the physical device and stores a desktop on a remote server
    - allows users to access their desktops from anywhere on any device