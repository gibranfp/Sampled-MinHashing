Sampled-MinHashing
==================
Sampled Min-Hashing (SMH) is a simple and scalable method to discover patterns from large-scale dyadic data (e.g. bag of words). SMH relies on Min-Hashing to efficiently mine beyond-pairwise relationships which are clustered to form the final discovered patterns. SMH has been successfully applied to the discovery of objects from image collections and topics from text corpora. This repository includes a C implementation of SMH together with SWIG Python bindings. 

## Installation
Install the dependencies:
~~~~	
sudo apt-get install cmake python swig libpython-dev
~~~~

Clone and coompile the library:
~~~~
git clone https://github.com/gibranfp/Sampled-MinHashing.git
cd Sampled-MinHashing
mkdir build
cd build
cmake ..
make
~~~~

To do a system-wide installation
~~~~
sudo make install
~~~~

Alternatively, you can use it locally by adding the absolute path of the `bin` directory inside `Sampled-MinHashing` to the system path:
~~~~
export PATH=$PATH:[absolute_path_to_sampled_minhashing]/bin
~~~~

And the absolute path of the `python/smh` directory inside `build` to Python's path:
~~~~
export PYTHONPATH=$PYTHONPATH:[absolute_path_to_sampled_minhashing]/build/python/smh
~~~~

To uninstall the library from your system do:
~~~~
sudo make uninstall
~~~~

## Example Usage
###  Getting NIPS Corpus
To discover topics from the NIPS corpus using Sampled-MinHashing, first download and extract the corpus to a given location:
~~~~
wget http://arbylon.net/projects/nips/nips-20110223.zip
unzip nips-20110223.zip
~~~~

This creates the directory `knowceans-ilda/nips` where the corpus is located. The file `nips.corpus` inside this directory contains a database of N lists corresponding  to the bag-of-words of the N documents in the corpus. The format of the file is as follows:
~~~~
size_of_list_1 item1_1:freq1_1 item2_1:freq2_1 ...
size_of_list_1 item1_2:freq1_22 item2_2:freq2_2 ...
...                        ...
size_of_list_N item1_N:freq1_N item2_N:freq2_N ...
~~~~

For example, if you have a corpus of 5 documents with a vocabulary of 19 different term, the file could look like this:
~~~~
6 3:9 4:8 7:5 12:1 16:5 18:5 
3 2:7 3:4 8:5
4 1:9 2:10 16:8 17:10
4 10:10 11:4 15:8 16:3
3 0:1 14:9 15:10
~~~~

### Creating Inverted File Structure
To perform topic discovery you'll need to load the corpus and create the inverted file structure. This can be done using the standalone command smhcmd:
~~~~
smhcmd ifindex knowceans-ilda/nips/nips.corpus knowceans-ilda/nips/nips.ifs
~~~~

Or from Python:
~~~~
import smh
corpus = smh.listdb_load('knowceans-ilda/nips/nips.corpus')
ifs = corpus.invert()
ifs.save('knowceans-ilda/nips/nips.ifs')
~~~~

### Discovering Topics
Once you have the inverted file,  to discover topics from the standalone smhcmd command you need to do
~~~~
smhcmd discover~/knowceans-ilda/nips/nips.ifs ~/knowceans-ilda/nips/nips.models
~~~~

From Python:
~~~~
import smh
corpus = smh.listdb_load('knowceans-ilda/nips/nips.corpus')
ifs = smh.listdb_load('knowceans-ilda/nips/nips.ifs')
discoverer = smh.SMHDiscoverer()
models = discoverer.fit(ifs, expand = corpus)
models.save('knowceans-ilda/nips/nips.models')
~~~~

To visualize the topics as sets of terms, load the vocabulary file and map term IDs to terms:
~~~~
vocabulary = {}
with open('knowceans-ilda/nips/nips.vocab', 'r') as f:
	content = f.readlines()
	for line in content:
        	tokens = line.split(' = ')
        	vocabulary[int(tokens[1])] = tokens[0]

topics = []
for m in models.ldb:
	terms = []
	for j in m:
        	terms.append(vocabulary[j.item])
	topics.append(terms)
~~~~

Finally save the lists of terms to a file:
~~~~
with open('knowceans-ilda/nips/nips.terms', 'w') as f:
	for t in topics:
		f.write(' '.join(t).encode('utf8'))
		f.write('\n'.encode('utf8'))
~~~~

## References
* Gibran Fuentes-Pineda and Ivan Vladimir Meza-Ruiz. [**Topic Discovery in Massive Text Corpora Based on Min-Hashing**](https://www.sciencedirect.com/science/article/abs/pii/S0957417419304245). Expert Systems with Applications, 136, 62--72, 2019. Preprint available at [https://arxiv.org/abs/1807.00938](https://arxiv.org/abs/1807.00938).
* Gibran Fuentes Pineda, Hisashi Koga and Toshinori Watanabe. [**Scalable Object Discovery: A Hash-based Approach to Clustering Co-occurring Visual Words**](http://turing.iimas.unam.mx/~gibranfp/ieice2011b.pdf). Transactions on Information and Systems, E94-D(10):2024–2035, 2011.
