# ngram
An n-gram model for language generation created using the Open American National Corpus.

<sup><sub>that's a lie it actually only does trigrams</sub></sup>

Compile `ngrams.cpp` using the c++17 standard and the compiler of your choice, and running the program will prompt you to give a few seeded words to use in generating a sentence.

# The OANC
"The Open American National Corpus is a roughly 15 million word subset of the ANC Second Release that is unrestricted in terms of usage and redistribution." - [ANC Website](https://anc.org/data/oanc/)

Parsing such a large dataset is expensive. Compounded with my probably-noob-ish skill with C++, this means that the OANC files take a long time to fully read and load into the model. To somewhat combat this, a method for storing the parsed data into a cache file has been given to speed up each run of the program.
```cpp
NGramModel model = NGramModel();
model.load();
model.cache_ngrams(); // Only needs to be run once to store parsed data.
```
On every subsequent `load()` call, the program will search for the `ngrams.cache` file, and if it is not found, will default to manually parsing the data. The `main()` method of `ngrams.cpp` also has a commented out line that can be uncommented to generate the cache on the first run, after which can be commented out again to prevent excess file-writes.

### Notes
> The following timings were calculated on my (Windows) machine:

> Full read and load takes about 64 seconds, which is reduced to 47 seconds with the cache (not much, but not sure how to make it faster)

> Using the `-O1` flag can reduce runtime to around 28 seconds, with negligible reduction in runtime with cache

> On my Linux machine (running Ubuntu 20.04), the `-O1` flag with cache led to parsing times as low as 6 seconds

# Predicting Words / Generating Sentences
Given a specific context (of two words), the model is able to predict words, or generate a string until it can no longer generate further:
```cpp
NGramModel model = NGramModel();
model.load();
cout << model.predict("it is") << '\n'; // Will give a prediction of the next word.
cout << model.gen_sentence("it is") << '\n'; // Pastes together predictions until it cannot anymore.
```
And it can also give a probability for a word to be predicted given a specific context:
```cpp
cout << model.ngram_prob("it is", "not") << '\n'; // .06... or something
```

# Manually Adding Data
The NGramModel class includes a method `update_model` that allows you to add your own data to the model. `update_model` takes a vector of strings, which can be acquired using the `tokenize` static member-method of the NGramModel class.
```cpp
NGramModel model = NGramModel();
std::ifstream file("./mydata.txt");
vector<string> words{};
string text;
while(getline(file, text)) {
    words = NGramModel::tokenize(text + " "); // Extra space is added so tokenize can find last word.
    model.update_model(words);
}
// You can also cache your user-added data.
// Make sure you also use model.load() if you want the OANC data in addition to your data.
model.cache_ngrams();
```
