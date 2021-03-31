#!/usr/bin/python
# DS-System Web Timers: converter from HTML-embedded JS to CPP-embedded JS
# Based on https://javascript-minifier.com/python
# (c) DNS 2021

import sys
import requests

# main()
try:
    html_file = sys.argv[1]
except:
    print("Missing input file. Syntax: js2cpp.py html-with-js.html")
    sys.exit()

# Cut off JS from HTML
js = ""
with open(html_file, 'r') as f:
    line = f.readline()
    in_js = False
    while line:
        if "= START JAVASCRIPT =" in line:
            in_js = True
            line = f.readline()
        if "= END JAVASCRIPT =" in line:
            in_js = False
            line = f.readline()
        if in_js:
            js += line
        line = f.readline()

# Compress
payload = {"input": js}
url = "https://javascript-minifier.com/raw"
## Reencode UTF-8 back to HTML entities, as UTF-8 icons appear ugly in Arduino IDE
jsm = requests.post(url, payload).text.encode('ascii', 'xmlcharrefreplace').decode()

# Output as C-string
width = 120
count=0
print('static const char *{}_script PROGMEM = "<script>"'.format(html_file.rstrip(".html")))
for i in range(len(jsm)):
    if (count == 0):
        print('  "', end = "")
    if (jsm[i] == '"'):
        print("\\", end = "");
        count += 1
    print(jsm[i], end = "")
    count += 1
    if (count >= width):
        count = 0
        print('"')
if (count != 0):
    print('"', end = "")
print(";")
