#-------setup c++



# #-------setup python ---------
export SKLEARN_ALLOW_DEPRECATED_SKLEARN_PACKAGE_INSTALL=True
python3 -m venv .venv
sleep 5
source .venv/bin/activate
sleep 2
pip3 install -r requirements.txt
