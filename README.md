# Spam Filter

수학식만 고친다면 될 것 같음.  
나중에 하기 위해 소스를 남겨 놓음.  

#### ReadLine.c
datasets.zip을 압축해서 나온 datasets들에 대해,  
csv파일들의 각 번호들의 내용을 읽을 수 있는 소스.  

#### Tokenizing.c
csv파일들을 읽고 난 후, 각각에 대해 word단위로 tokenizing 가능한 소스.  

#### SpamFilter.c
tokenizied된 word들을 가지고, test파일을 읽어서, spamfilter를 적용하는 소스.  
단, 현재 계산식이 잘못된 것 같다.  
추후에 다시 `findProbability`함수를 수정해야 함.  