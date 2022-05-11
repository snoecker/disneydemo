# disneydemo

#to run on a clean centos/rocky/rhel image/container, ubuntu will need to use apt-get and probably change some names

#the first two may be actually included in dev tools later, but this is order I did
sudo dnf install cmake 
sudo dnf install build-essential

sudo dnf group install "Development Tools"
sudo dnf install openssl-devel

git clone https://github.com/snoecker/disneydemo.git

cmake CMakeLists.txt
make
./disney

