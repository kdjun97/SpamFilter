// Tokenizing까지 가능한 스크립트


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

void readModelData(char path[], char pattern[], read_model* t);
int numCheck(char pattern[], char* str, int ct, read_model* t);
void showStructureData(read_model t);
void showTokenWord(word_model t);
void parseToken(read_model* t, word_model* tword, int num);

int main(void)
{
    // readModelData(TRAIN_HAM_PATH, ",ham,", ham_train);
    readModelData(TRAIN_SPAM_PATH, ",spam,", spam_train);
    // for (int i=1; i<=100; i++)
    //     showStructureData(spam_train[i]);
    
    parseToken(spam_train, spam_train_word, 100);
    for (int i=1; i<=100; i++)
        showTokenWord(spam_train_word[i]);

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

// structure에 잘 들어갔는지 print를 위한 함수
void showStructureData(read_model t)
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