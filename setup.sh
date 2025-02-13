#-------setup c++

mkdir build
cd build
cmake ..
make -j16
cd ..

# #-------setup python ---------
python3 -m venv .venv
sleep 5
source .venv/bin/activate
sleep 2
pip3 install -r requirements.txt
