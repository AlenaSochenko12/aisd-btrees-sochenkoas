#ifndef btree_H
#define btree_H

#include <iostream>
#include <conio.h>
#include <fstream>

using namespace std;

#define MAX 4
#define MIN 2

struct btreeNode {
    int val[MAX + 1], count;
    btreeNode* link[MAX + 1];
};

btreeNode* root;

//�������� ������ ����
btreeNode* createNode(int val, btreeNode* child) {
    btreeNode* newNode = new btreeNode;
    newNode->val[1] = val;
    newNode->count = 1;
    newNode->link[0] = root;
    newNode->link[1] = child;
    return newNode;
}

//�������� �������� �� ��������������� �������
void addValToNode(int val, int pos, btreeNode* node, btreeNode* child) {
    int j = node->count;
    while (j > pos) {
        node->val[j + 1] = node->val[j];
        node->link[j + 1] = node->link[j];
        j--;
    }
    node->val[j + 1] = val;
    node->link[j + 1] = child;
    node->count++;
}

//���������� ����
void splitNode(int val, int* pval, int pos, btreeNode* node, btreeNode* child, btreeNode** newNode) {
    int median, j;

    if (pos > MIN)
        median = MIN + 1;
    else
        median = MIN;

    *newNode = new btreeNode;
    j = median + 1;
    while (j <= MAX) {
        (*newNode)->val[j - median] = node->val[j];
        (*newNode)->link[j - median] = node->link[j];
        j++;
    }
    node->count = median;
    (*newNode)->count = MAX - median;

    if (pos <= MIN) {
        addValToNode(val, pos, node, child);
    }
    else {
        addValToNode(val, pos - median, *newNode, child);
    }
    *pval = node->val[node->count];
    (*newNode)->link[0] = node->link[node->count];
    node->count--;
}

//��������� �������� � ����
int setValueInNode(int val, int* pval, btreeNode* node, btreeNode** child) {

    int pos;
    if (!node) {
        *pval = val;
        *child = NULL;
        return 1;
    }

    if (val < node->val[1]) {
        pos = 0;
    }
    else {
        for (pos = node->count;
            (val < node->val[pos] && pos > 1); pos--);
        if (val == node->val[pos]) {
            cout << "Duplicates not allowed\n";
            return 0;
        }
    }
    if (setValueInNode(val, pval, node->link[pos], child)) {
        if (node->count < MAX) {
            addValToNode(*pval, pos, node, *child);
        }
        else {
            splitNode(*pval, pval, pos, node, *child, child);
            return 1;
        }
    }
    return 0;
}

//������� � ������
void insertion(int val) {
    int flag, i;
    btreeNode* child;

    flag = setValueInNode(val, &i, root, &child);
    if (flag)
        root = createNode(i, child);
}

//�������� ��������� ��� ���������� ��������
void copySuccessor(btreeNode* myNode, int pos) {
    btreeNode* dummy;
    dummy = myNode->link[pos];

    for (; dummy->link[0] != NULL;)
        dummy = dummy->link[0];
    myNode->val[pos] = dummy->val[1];

}

//������� �������� �� ��������� ���� � ������������ ��������
void removeVal(btreeNode* myNode, int pos) {
    int i = pos + 1;
    while (i <= myNode->count) {
        myNode->val[i - 1] = myNode->val[i];
        myNode->link[i - 1] = myNode->link[i];
        i++;
    }
    myNode->count--;
}

//�������� �������� �� �������� � ������� �������
void doRightShift(btreeNode* myNode, int pos) {
    btreeNode* x = myNode->link[pos];
    int j = x->count;

    while (j > 0) {
        x->val[j + 1] = x->val[j];
        x->link[j + 1] = x->link[j];
    }
    x->val[1] = myNode->val[pos];
    x->link[1] = x->link[0];
    x->count++;

    x = myNode->link[pos - 1];
    myNode->val[pos] = x->val[x->count];
    myNode->link[pos] = x->link[x->count];
    x->count--;
    return;
}

//�������� �������� �� �������� � ������ �������
void doLeftShift(btreeNode* myNode, int pos) {
    int j = 1;
    btreeNode* x = myNode->link[pos - 1];

    x->count++;
    x->val[x->count] = myNode->val[pos];
    x->link[x->count] = myNode->link[pos]->link[0];

    x = myNode->link[pos];
    myNode->val[pos] = x->val[1];
    x->link[0] = x->link[1];
    x->count--;

    while (j <= x->count) {
        x->val[j] = x->val[j + 1];
        x->link[j] = x->link[j + 1];
        j++;
    }
    return;
}

//����������� �����
void mergeNodes(btreeNode* myNode, int pos) {
    int j = 1;
    btreeNode* x1 = myNode->link[pos], * x2 = myNode->link[pos - 1];

    x2->count++;
    x2->val[x2->count] = myNode->val[pos];
    x2->link[x2->count] = myNode->link[0];

    while (j <= x1->count) {
        x2->count++;
        x2->val[x2->count] = x1->val[j];
        x2->link[x2->count] = x1->link[j];
        j++;
    }

    j = pos;
    while (j < myNode->count) {
        myNode->val[j] = myNode->val[j + 1];
        myNode->link[j] = myNode->link[j + 1];
        j++;
    }
    myNode->count--;
    free(x1);
}

//������������� ����
void adjustNode(btreeNode* myNode, int pos) {
    if (!pos) {
        if (myNode->link[1]->count > MIN) {
            doLeftShift(myNode, 1);
        }
        else {
            mergeNodes(myNode, 1);
        }
    }
    else {
        if (myNode->count != pos) {
            if (myNode->link[pos - 1]->count > MIN) {
                doRightShift(myNode, pos);
            }
            else {
                if (myNode->link[pos + 1]->count > MIN) {
                    doLeftShift(myNode, pos + 1);
                }
                else {
                    mergeNodes(myNode, pos);
                }
            }
        }
        else {
            if (myNode->link[pos - 1]->count > MIN)
                doRightShift(myNode, pos);
            else
                mergeNodes(myNode, pos);
        }
    }
}

//�������� �������� �� ����
int delValFromNode(int val, btreeNode* myNode) {
    int pos, flag = 0;
    if (myNode) {
        if (val < myNode->val[1]) {
            pos = 0;
            flag = 0;
        }
        else {
            for (pos = myNode->count;
                (val < myNode->val[pos] && pos > 1); pos--);
            if (val == myNode->val[pos]) {
                flag = 1;
            }
            else {
                flag = 0;
            }
        }
        if (flag) {
            if (myNode->link[pos - 1]) {
                copySuccessor(myNode, pos);
                flag = delValFromNode(myNode->val[pos], myNode->link[pos]);
                if (flag == 0) {
                    cout << "\n������� �� ������ � ������\n";
                }
            }
            else {
                removeVal(myNode, pos);
            }
        }
        else {
            flag = delValFromNode(val, myNode->link[pos]);
        }
        if (myNode->link[pos]) {
            if (myNode->link[pos]->count < MIN)
                adjustNode(myNode, pos);
        }
    }
    return flag;
}

//�������� �������� �� ������
void deletion(int val, btreeNode* myNode) {
    btreeNode* tmp;
    if (!delValFromNode(val, myNode)) {
        cout << "\n������� �� ������ � ������\n";
        return;
    }
    else {
        if (myNode->count == 0) {
            tmp = myNode;
            myNode = myNode->link[0];
            free(tmp);
        }
        cout << "\n����� ������� �������!";
    }
    root = myNode;
    return;
}

//����� �������� � ������
void searching(int val, int* pos, btreeNode* myNode) {
    if (!myNode) {
        return;
    }

    if (val < myNode->val[1]) {
        *pos = 0;
    }
    else {
        for (*pos = myNode->count;
            (val < myNode->val[*pos] && *pos > 1); (*pos)--);
        if (val == myNode->val[*pos]) {
            cout << "\n������� ������\n";
            return;
        }
        else {
            cout << "\n������� �� ������\n";
            return;
        }
    }
    searching(val, pos, myNode->link[*pos]);
    return;
}

//����� ������
void traversal(btreeNode* myNode) {
    int i;
    if (myNode) {
        for (i = 0; i < myNode->count; i++) {
            traversal(myNode->link[i]);
            cout << myNode->val[i + 1] << ' ';
        }
        traversal(myNode->link[i]);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    for (int i = 0; i < 20; i++) {
        char in_f[400];
        sprintf(in_f, "test/%i/in.txt", i);
        FILE* in_file = fopen(in_f, "r");
    }
    int val, opt;
    while (true) {
        cout << " 1. ������� ��������\n 2. �������� ��������\n 3. ����� ��������\n 4. ���������� ������\n 5. �����\n  ������� ����� ��������:";
        cin >> opt;
        cout << endl;
        switch (opt) {
        case 1:
            cout << "������� �����: ";
            cin >> val;
            insertion(val);
            cout << "\n����� ������� ���������!\n";
            break;
        case 2:
            cout << "������� �������, ������� ���������� �������: ";
            cin >> val;
            deletion(val, root);
            break;
        case 3:
            cout << "������� �������, ������� ���������� �����: ";
            cin >> val;
            searching(val, &opt, root);
            break;
        case 4:
            traversal(root);
            break;
        case 5:
            exit(0);
        }
        cout << endl;
    }
    system("pause");
}
#endif