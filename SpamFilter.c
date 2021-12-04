// Spam Filter
// 생각한 로직
/*
1. train data를 다 읽음(각각)
2. 읽을때 한줄씩 읽고, ",ham," 혹은 ",spam,"이라는 문자열이 line에 있는지 검사. (첫번째 줄은 num, label, text여서 continue 할 예정)
(많은 방법이 있겠지만, notepad++로 csv파일 4개를 확인해본 결과, 이 과제에서는 ",ham,"과 ",spam,"이라는 연속적인 문자로 split이 가능하기 때문에 이 방법을 사용.)
    2-1. 찾았다면, split을 ,, 두개 기준으로 첫번째껀 num에 두번째는 label 마지막은 text에 넣음.
    2-2. 또 line에 특정 문자를 검사하면서, 없으면 strcat으로 계속 text를 이어줌.
    2-3. line에서 특정 문자가 발견되었다면, count++을 하고, structure 배열도 ++해주면서 전부 넣음.
3. 각 structure배열마다 단어를 split해준다. Tokenizing
4. 가지고있는 Tokenzied 된 단어들로 수학적인 계산을 한다 (word를 찾는 함수를 만들어서, p값 q값을 계산하게 하면 될듯) p,q값을 return해주는 함수 2개 필요할듯.
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define TRAIN_HAM_PATH "datasets/datasets/train/dataset_ham_train100.csv"
#define TRAIN_SPAM_PATH "datasets/datasets/train/dataset_spam_train100.csv"
#define TEST_HAM_PATH "datasets/datasets/test/dataset_ham_test20.csv"
#define TEST_SPAM_PATH "datasets/datasets/test/dataset_spam_test20.csv"
#define MAX 9999 // 이 역시 메모리 낭비.
#define MAX_LINE_LENGTH 1024 // 문제점. 안전하게 8192로 했지만, 이것은 리소스 낭비. 보고서에 개선할 점으로 적어야 함.
#define MAX_WORD_LENGTH 2400
#define LL long long
#define ld long double

// 구조체 생성 (train, test line 정보를 담을 모델 생성)
// MAX 부분 보고서에 적을 수 있음. malloc으로 동적 할당하면 더 빨라짐.
typedef struct READ_MODEL {
    int num;
    int label; // 0 : ham, 1 : spam
    char text[MAX]; // text -> train spam에 64번이 9915글자여서 9999로 MAX를 잡음.
} read_model;

// tokenized된 word만 가지고 있는 모델
typedef struct WORD_MODEL {
    int num;
    int label; // 0 : ham, 1 : spam
    int count; // word's count
    char text[MAX_WORD_LENGTH][100];
} word_model;

read_model ham_train[101];
read_model spam_train[101];
read_model ham_test[21];
read_model spam_test[21];
word_model ham_train_word[100];
word_model spam_train_word[100];
word_model ham_test_word[20];
word_model spam_test_word[20];
int ham_train_all_words=0;
int spam_train_all_words=0;
int ham_test_all_words=0;
int spam_test_all_words=0;

void readModelData(char path[], char pattern[], read_model* t);
int numCheck(char pattern[], char* str, int ct, read_model* t);
void showMailData(read_model t);
void showTokenWord(word_model t);
void parseToken(read_model* t, word_model* tword, int num);
int returnNum(char temp[]);
int findCount(char word[], word_model* tword);
int findAllWordsCount(word_model* tword, int num);
double findProbability(word_model test_word, word_model* h_train_word, word_model* s_train_word); // r 구하기
double findAccuracy(int correct);

int main(void)
{
    readModelData(TRAIN_HAM_PATH, ",ham,", ham_train);
    readModelData(TRAIN_SPAM_PATH, ",spam,", spam_train);
    readModelData(TEST_HAM_PATH, ",ham,", ham_test);
    readModelData(TEST_SPAM_PATH, ",spam,", spam_test);
    
    parseToken(ham_train, ham_train_word, 100);
    parseToken(spam_train, spam_train_word, 100);
    parseToken(ham_test, ham_test_word, 20);
    parseToken(spam_test, spam_test_word, 20);

    ham_train_all_words = findAllWordsCount(ham_train_word, 100);
    spam_train_all_words = findAllWordsCount(spam_train_word, 100);

    for (int j=0; j<20; j++)
    {
        double r1 = findProbability(ham_test_word[j], ham_train_word, spam_train_word);
        printf("Probability for r%d = %f\n",j+1 ,r1);
    }
    for (int j=0; j<20; j++)
    {
        double r1 = findProbability(spam_test_word[j], ham_train_word, spam_train_word);
        printf("Probability for r%d = %f\n",j+1 ,r1);
    }

    printf("%f\n",findAccuracy(32));

    // for (int i=1; i<=20; i++)
    //     showMailData(ham_test[i]);
    // for (int i=1; i<=20; i++)
    //     showMailData(spam_test[i]);
    // for (int i=1; i<=100; i++)
    //     showMailData(ham_train[i]);
    // for (int i=1; i<=100; i++)
    //     showMailData(spam_train[i]);

    // for (int i=0; i<100; i++)
    //     showTokenWord(ham_train_word[i]);
    // for (int i=0; i<100; i++)
    //     showTokenWord(spam_train_word[i]);
    // for (int i=0; i<20; i++)
    //     showTokenWord(ham_test_word[i]);
    // for (int i=0; i<20; i++)
    //     showTokenWord(spam_test_word[i]);

    printf("End");
    return 0;
}

// csv 하나를 읽고, 그 csv에 대한 모든 read_model에 값을 넣어주는 역할.
void readModelData(char path[], char pattern[], read_model* t)
{
    printf("---------FILE READ START----------\n");
    printf("FILE INFO : %s\n", path);
    printf("Pattern INFO : %s\n", pattern);

    FILE *fp = fopen(path, "r");
    int ct=-1;

    if (fp != NULL)
    {
        while(!feof(fp))
        {
            char lines[MAX_LINE_LENGTH];
            char *str = fgets(lines, MAX_LINE_LENGTH, fp);
            if (str == NULL)
            {
                fclose(fp);
                break;
            }
            // printf("[%d] :%s",ct, str);
            ct = numCheck(pattern, str, ct, t);
        }
        fclose(fp);
    }
    printf("----------FILE READ END-----------\n");
}

// 자를 부분을 반환. 없으면 0
// ",ham," ",spam,"을 체크하는 부분. 
// 현재 구조체 배열의 index값도 넘겨받아야함.
int numCheck(char pattern[], char* str, int ct, read_model* t)
{
    char* temp = strstr(str, pattern);
    if (temp != NULL) // 찾았다면,
    {
        // printf("Find!\n");
        ct++;

        int num, label; // label:0 = ham / label:1 = spam
        
        char* tok;
        // printf("TARGET:%s", str);

        tok = strtok(str, ",");
        num = atoi(tok);
        if (num == 0) // s01이나 h01같은 문자를 만나면, 0이 반환됨.
            num = returnNum(tok);
        
        tok = strtok(NULL, ",");

        if (strcmp(tok, "ham")==0)
            label=0;
        else if (strcmp(tok, "spam")==0)
            label=1;
        else // exception
            label=-1;

        tok = strtok(NULL, "\n");
        tok[strlen(tok)]='\n'; // 아래에서 strcat할때, 개행문자가 포함되지 않음. 따라서 강제적으로 넣어줌.
        t[ct].num=num;
        t[ct].label=label;
        strcpy(t[ct].text, tok);

        // printf("[ct=%d][%d][%d][%s]\n",ct, t[ct].num, t[ct].label, t[ct].text);
    }
    else
    {
        if (ct == -1)
            ct++;
        else
            strcat(t[ct].text, str);
    }
    return ct;
}

// 각 메일에 잘 들어갔는지 print를 위한 함수
void showMailData(read_model t)
{
    printf("[%d]---Label:%d----\n",t.num, t.label);
    printf("%s\n",t.text);
    printf("-------------------\n");
}

// tokenizing이 잘 되었는지 검사하는 함수
void showTokenWord(word_model t)
{
    int compare_ct=0, i=0;
    printf("num=[%d]\nLabel:[%d]\n",t.num, t.label);
    printf("Tokenizing...\n");
    while (1)
    { 
        printf("[%s]", t.text[i]);
        if (i == t.count)
            break;
        else
            i++;
    }
    printf("End of Token!\n");
}

// num 하나당 word를 뽑아내는 함수
void parseToken(read_model* t, word_model* tword, int num)
{
    // printf("---Tokenizing---\n");
    for (int i=1, j=0; i<=num; i++, j++)
    {
        // printf("For token :\n%s", t[i].text);
        int count=0;
        char tokenize[] = " \n";
        char* token;

        tword[j].num=t[i].num;
        tword[j].label=t[i].label;

        token = strtok(t[i].text, tokenize);

        while (token != NULL)
        {
            strcpy(tword[j].text[count],token);
            count++;
            tword[j].count=count;
            token = strtok(NULL, tokenize);
        }        
    }
    // printf("---Tokenizing End---\n");
}

// s01같이 문자와 숫자가 섞였을 때, 숫자만 return해주는 함수
int returnNum(char temp[])
{
    char split_temp[2];
    split_temp[0]=temp[1];
    split_temp[1]=temp[2];
    return atoi(split_temp);
}

// 단어가 하나 들어왔을 때, 2번째 인자의 구조체 배열에 몇번 등장했는지 알려주는 함수.
// 단어 완전 일치 감지.
int findCount(char word[], word_model* tword)
{
    int count=0;
    for (int i=0; i<100; i++)
    {
        for (int j=0; j<tword[i].count; j++)
        {
            if (strcmp(tword[i].text[j], word) == 0)
                count++;
        }
    }
    return count;
}

// 모든 word의 갯수를 반환해주는 함수
int findAllWordsCount(word_model* tword, int num)
{
    int all_words_count=0;

    for (int i=0; i<num; i++)
        all_words_count+=tword[i].count;

    return all_words_count;
}

// 확률을 구해주는 함수
double findProbability(word_model test_word, word_model* h_train_word, word_model* s_train_word) // r 구하기
{
    double p=1.0;
    double q=1.0;
    double result;

    printf("------Finding Probability Start-----\n");
    
    for (int i=0; i<test_word.count; i++)
    {
        double temp_p, temp_q;
        int foundWordCountH = findCount(test_word.text[i], h_train_word); // h_train_word에서 빈도수 찾기
        int foundWordCountS = findCount(test_word.text[i], s_train_word); // s_train_word에서 빈도수 찾기
        if (foundWordCountH == 0 || foundWordCountS == 0) // p 혹은 q를 구할 때, 0이면 devide by zero때문에 continue를 시켜줌.
            continue;
        
        temp_p = (double)foundWordCountS / spam_train_all_words;
        temp_q = (double)foundWordCountH / ham_train_all_words;
        p*=temp_p;
        q*=temp_q;

    }
    if ((p+q)!=0)
        result = p/(p+q);
    else
        result=0;

    printf("------Finding Probability End-------\n");
    return result;
}

// 보고서에 쓸 accruacy를 위한 함수.
double findAccuracy(int correct)
{
    return ((double)correct/40)*100;
}