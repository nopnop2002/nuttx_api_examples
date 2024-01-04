# How to use   

Your nuttx directory is $HOME/nuttxspaces.   
Copy all file to your $HOME/nuttxspaces/apps/nuttx_api_example directory and add all examples.   


```
$ git clone https://github.com/nopnop2002/nuttx_api_examples   
$ cd nuttx_api_examples   
$ echo "source \"$HOME/nuttxspace/apps/nuttx_api_example/Kconfig\"" >> $HOME/nuttxspace/apps/Kconfig
$ mkdir $HOME/nuttxspace/apps/nuttx_api_example
$ cp -r nuttx_api_* $HOME/nuttxspace/apps/nuttx_api_example/
$ sed s@HOME@$HOME/nuttxspace/apps/nuttx_api_example@g Kconfig > $HOME/nuttxspace/apps/nuttx_api_example/Kconfig
$ cd $HOME/nuttxspace/nuttx
$ make clean   
$ make menuconfig    
```
![config-1](https://github.com/nopnop2002/nuttx_api_examples/assets/6020549/3e56a67f-2a4b-4044-b889-949114e8c3f3)
![config-2](https://github.com/nopnop2002/nuttx_api_examples/assets/6020549/b0d6fcea-130b-4b24-b2e8-2c5ecf936692)
![config-3](https://github.com/nopnop2002/nuttx_api_examples/assets/6020549/8760535e-5fd5-42ab-a946-b0156d13cfbc)


If you find any bug, please tell me via issues.   
