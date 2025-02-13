wget -P ../data https://archive.ics.uci.edu/static/public/417/character+font+images.zip 
python3 download_SVHN.py
cd ../data
unzip character+font+images.zip
cd ../python
python3 data_to_json.py



