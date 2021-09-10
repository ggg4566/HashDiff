# HashDiff
应急响应辅助工具，根据样本hash 全盘检索

win10 vs2010 vc++ mfc

运行平台：OS >=win7   NTFS 格式硬盘 

# 功能

## 文件快速搜索

类似everything的搜索功能，第一次搜索需要花费时间建立索引，代码和原理：

https://github.com/LeiHao0/Fake-Everything

![](https://user-images.githubusercontent.com/7532477/132863751-fed089a4-f361-45b2-a006-698ec3cc4c18.png)


## 根据hash检索文件

a)支持全盘以及特定文件检索

b) 使用windows线程池加快检索速度,不要设置太高，默认线程数量 10～30

c) 在已知样本的情况下可以设置样本的大小来加快搜索速度

![](https://user-images.githubusercontent.com/7532477/132863853-545337a9-73a5-458d-9823-07d5b5afaba1.png)


## hash 计算

![](https://user-images.githubusercontent.com/7532477/132863936-b804bac3-9002-4409-80fc-2b45ed91cd5a.png)



# 致谢：

https://github.com/LeiHao0/Fake-Everything

https://github.com/cpp518/HashCalculator


