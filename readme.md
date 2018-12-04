### KENE(Konan English kNowledge Extractor)

This repository contains codes and sample data for English knowledge extraction from Friends captions.

### Requirements

* KEMA(Konan English Morphology Analyzer)
* KEPT(Konan English Pos Tagger)
* PCRE library
* GCC

### Building

````
$ make
````

### Input Format

Each Friends caption entry must be given in a single line as following format similar to that of CSV.

````
<period_num>,<start_time>,<end_time>,<speaker>,<caption>
  - period_num: period number
  - start_time: period start time
  - end_time: period end time
  - speaker: caption speaker
  - caption: English caption
````

* **Example**
````
6,0:01:08,0:01:11,Monica,Okay, everybody relax. This is not even a date.
````

See the file "dat/input" for more examples.

### Output Format

The extracted knowledge is a JSON array consists of caption analysis objects.
The object is represented as following format. 

````
 {
  "period_num": "period number", 
  "start_time": "period start time", 
  "end_time": "period end time", 
  "speaker": "caption speaker",
  "caption": "English caption",
  "results": [
   { 
    "pred": "predicate", 
    "args": [ "argument", ... ]
   }
   ...
  ]
 }
````
* **Example**
````
 {
  "period_num": "6",
  "start_time": "0:01:08",
  "end_time": "0:01:11",
  "speaker": "Monica",
  "caption": "Okay, everybody relax. This is not even a date.",
  "results": [
   {
    "pred": "is not",
    "args": [ "This", "even a date" ]
   }
  ]
 }
````

See the file "dat/output.json" for more examples.

### Running

* **Usage**
````
kene data-path config-file input-file output-file
````

'config-file', 'input-file' and 'output-file' must be relative to 'data-path'.

* **Example**
````
$ export LD_LIBRARY_PATH=./lib:./lib/pcre:$LD_LIBRARY_PATH
$ ./bin/kene dat kene.rc input output.json
````

See the file "kene.sh" for the running example.

See the file "dat/kene.rc" for the example of KENE configuration.

### Acknowledgements

This work was supported by Institute for Information & communications Technology Promotion(IITP) grant funded by the Korea government(MSIT) (2017-0-01780, The technology development for event recognition/relational reasoning and learning knowledge based system for video understanding)

