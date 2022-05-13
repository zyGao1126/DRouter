# DRouter
A simple VLSI detailed-routing tool based on A* Algorithm

## 1.how to build
step 1: Download the source code
```
$ git clone https://github.com/zyGao1126/DRouter
```
step 2: Go to the project root and bulid by
```
$ cd DRouter
$ mkdir result
$ make
```

## 2. how to run
```
$ chmod u+x ./run.sh
$ ./run.sh
```
Then a result.def will be generated in result folder, which can be visualized using EDA tools.

## 3. demo
we test the routing result in Innovus, which gives:

<img src="https://github.com/zyGao1126/DRouter/blob/master/solution1.png.PNG" width="400" height="400" />

<img src="https://github.com/zyGao1126/DRouter/blob/master/solution2.png.PNG" width="400" height="400" />
