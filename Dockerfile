# FROM：開指定的image    專案名稱：(分支名稱)
FROM ubuntu:14.04
# 作者
MAINTAINER Pei-Yao Chang  "bacon735392@gmail.com"
# 最新更新日期，只要改動此行，下面都會重建(越容易變動的東西都要寫在越下面)
ENV VERSION "2017-04-20 V1.0"

RUN \
  apt-get update && \
  apt-get install -y vim gcc g++ make

# Define mountable directories.
# 用來保存資料(設定檔、log檔、程式碼)
VOLUME ["./TCPIP"]

# Define working directory.
# 進container時，所在的目錄．
WORKDIR /


