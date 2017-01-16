git clone https://github.com/DQM4HEP/dim.git
cd dim
source setup.sh $1
source external-project/patch-command $1 # patch for mac
make GUI=no
cd ..
