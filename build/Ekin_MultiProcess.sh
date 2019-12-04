#!/bin/bash
# with this shell script, you can change the parameters in config files with a batch mode.
MainPath=$(pwd)
dire="./Run_"
Exec_Name="Hira_NW"

tmp_fifofile="/tmp/$$.fifo"
#建立了一个管道文件，并赋予了ID为6
mkfifo $tmp_fifofile
exec 6<>$tmp_fifofile
rm $tmp_fifofile

thread=10
for((i=0;i<$thread;i++));do
  echo
done>&6


for (( i = 5 ; $i <= 150; i++ ));
do
read -u6
{
  cd $MainPath;
  if [ -d "$dire$i" ]; then
    rm -rf "$dire$i"
  fi
  
  Certain_Ekin=$i
  echo "Certain_Ekin : " $i
  
  mkdir "$dire$i"
  cp -r ./Config $dire$i/
  cp -r ./macro  $dire$i/
  cp $Exec_Name  $dire$i/
  
  cd $MainPath/$dire$i/Config
  echo $PWD
#  read -p "Press any key to continue."
  sed -i  's/^Certain_Ekin.*/Certain_Ekin '${Certain_Ekin}'/' Source.config
  sed -i  's/^Output_FileName.*/Output_FileName 'Neutron_${Certain_Ekin}MeV_1E7.root'/' Initial.config
  
  cd $MainPath/$dire$i
  ./$Exec_Name

  echo >&6
}&

done

wait
#关闭6号文件
exec 6>&-
exit 0
