# How to use   

nuttx directory is $HOME/nuttxspaces.   
Copy all file to your $HOME/nuttxspaces/apps/examples directory and add all examples to Kconfig.   


```
$ git clone https://github.com/nopnop2002/nuttx_api_examples   
$ cd nuttx_api_examples   
$ cp -r nuttx_* $HOME/nuttxspace/apps/examples/
$ cp $HOME/nuttxspace/apps/examples/Kconfig $HOME/nuttxspace/apps/examples/Kconfig.old
$ sed s@HOME@$HOME/nuttxspace/apps/examples@g Kconfig >> $HOME/nuttxspace/apps/examples/Kconfig
$ cd $HOME/nuttxspace/nuttx
$ make menuconfig    
```

![config-1](https://github.com/nopnop2002/nuttx_api_examples/assets/6020549/30f7ac19-2eb6-40ab-9e99-24274ac2281c)
![config-2](https://github.com/nopnop2002/nuttx_api_examples/assets/6020549/3bcd79c2-f8b6-4b19-8c31-9abb1b25ab9d)
![config-3](https://github.com/nopnop2002/nuttx_api_examples/assets/6020549/8760535e-5fd5-42ab-a946-b0156d13cfbc)

```
make
st-flash --connect-under-reset write nuttx.bin 0x8000000
```

If you find any bugs, please let us know using issues.
