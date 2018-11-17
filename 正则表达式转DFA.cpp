#include <iostream>
#include <stack>
#include <vector>
#include<algorithm>
#include <queue>
#include<fstream>
using namespace std;

typedef struct STnode {     //�����﷨���Ľ��
char nodeVal; //����ֵ
int nodeId;  //Ψһ��ʶҶ�ӽڵ�
int nullable;
vector <int> firstpos;
vector <int> lastpos;
vector <int> followpos;
struct STnode *left;
struct STnode *right;
} STnode;

string input;
stack<STnode> Opt;
stack<STnode> Alp;
/*****************************�����﷨��***********************************/
int isOpt(char x)
{
    if(x>=65&&x<=90)
        return 0;
    else if (x>=97&&x<=122) //��д��Сд
        return 0;
    else
        return 1;
}

int compare(STnode A,STnode B)
{
    char a=A.nodeVal;
    char b=B.nodeVal;
    int x,y;
    if(a=='*') x=4;
    else if(a=='@') x=3;
    else if(a=='|') x=2;
    else if(a==')') x=1;
    else x=0;
    if(b=='*') y=4;
    else if(b=='@') y=3;
    else if(b=='|') y=2;
    else if(b==')') y=1;
    else if(b=='#') y=-1;
    else y=0;
    if(x==0&&y==1) return 0;//�˳�����
    if(x==0||y==0) return -1;
    else if(x>=y) return 1;//���ȼ���
    else return -1;//���ȼ��ͣ�ѹ

}

STnode syntexTree(string input)
{
    int len=input.length();
    for(int i=0;i<len;i++)
    {
        if(!isOpt(input[i])) //������������
        {
            STnode alpNode;
            alpNode.nodeVal=input[i];
            alpNode.left=NULL;
            alpNode.right=NULL;  //����ĸת��Ϊ���
            Alp.push(alpNode); //ѹ����ջ
        }
        else
        {
            STnode optNode;
            optNode.nodeVal=input[i];
            optNode.nodeId=-1;
            optNode.left=NULL;
            optNode.right=NULL;  //�������ת��Ϊ���
            if(Opt.empty())
                 Opt.push(optNode); //Ϊ��ֱ�Ӽ���
            else{
                STnode old=Opt.top();
                int res=compare(old,optNode);//ȡջ����������·��űȽ�
                while(res==1)  //ջ��������ȼ���
                {   Opt.pop(); //����ջ�����
                    STnode letter1;
                    STnode letter2;
                    if(old.nodeVal=='*') {
                        letter1=Alp.top();
                        Alp.pop();
                        old.left= new STnode (letter1);
                        old.right=NULL;
                        Alp.push(old);
                        }
                    else {
                        letter1=Alp.top(); Alp.pop();
                        letter2=Alp.top(); Alp.pop();
                    old.left= new STnode (letter2);//�������ȵ���������Ҫ������
                    old.right= new STnode (letter1); //���������new��ֵ�������µ��ڴ�����
    // old.left=&letter2; old.right=&letter1; ��������ָ�ֵ��ʽ���൱��û��������ַ������һֱ��ԭ��ַ�ϸ���ֵ�����Ի��������ѭ��
                        Alp.push(old);
                        }
                    if(Opt.empty()) {Opt.push(optNode); break;}
                    else{
                        old=Opt.top();
                        res=compare(old,optNode); //һֱ�Ƚ�ֱ���Ͻ�����ȼ����
                    }

                }
                if(res==-1)
                Opt.push(optNode);//�½�����ȼ��ߣ�ѹ��
                else if(res==0){
                    STnode test=Opt.top();
                    Opt.pop(); //����������
                }
            }

        }
    }
        STnode A=Alp.top(); Alp.pop();  //��󽫱��ʽ�ͽ�����#��@����
        STnode B=Opt.top(); Opt.pop();
        STnode optNode;
        optNode.nodeVal='@';
        optNode.nodeId=-2; //������ڵ�
        optNode.left=NULL;
        optNode.right=NULL;
        optNode.left=new STnode (A);  optNode.right=new STnode (B);
        STnode head=optNode;
        return head;//���ظ��ڵ�
}

/*****************************��ʼ����ΪҶ�����ӱ��*************************************/
int notenum=1;
void num(STnode *node) //��Ҷ�ӽڵ���ӱ��
{
    if(node!=NULL)
    {
        num(node->left);
        num(node->right);
        if(node->left==NULL&&node->right==NULL)
        {
            node->nodeId=notenum;
            notenum++;
        }
    }
    return ;
}

/**************************���������������ֵ��nullable,firstpos,lastpos*************************************/
int note_nullable(STnode *node)  //������ָ�����ͣ����ֱ�Ӵ���ڵ� �޷������ı���ֵ
{
    if(node->nodeVal=='*')
        node->nullable=1;
    else if(!isOpt(node->nodeVal)|node->nodeVal=='#')
        node->nullable=0;
    else{
        if(node->nodeVal=='|')
           node->nullable=(node->left)->nullable|(node->right)->nullable;
        if(node->nodeVal=='@')
           node->nullable=(node->left)->nullable&&(node->right)->nullable;
    }
    return 1;
}

int note_firstpos(STnode *node)
{
   if(!isOpt(node->nodeVal)|node->nodeVal=='#')
   {
       node->firstpos.push_back(node->nodeId);//��Ϊ�ַ�ֱ�Ӽ����Լ�
       return 0;
   }
   else{
        if(node->nodeVal=='*')
        {
            node->firstpos=(node->left)->firstpos;//��Ϊ* ͬ������
            return 0; //�����൱��break����Ȼ����ִ������Ĵ���ᱨ����Ϊ*û��������
        }

        vector<int> A=(node->left)->firstpos;
        vector<int> B=(node->right)->firstpos;
        vector<int> C;
        C=A;
     for(int i=0;i<B.size();i++)
     {   int flag;
         for(int j=0;j<A.size();j++)
         {
             if(A[j]==B[i]) flag=0; //�ҵ���ͬ�����0 �����
             else flag=1;
         }
         if(flag) C.push_back(B[i]); //����ͬ�ļ��룬�õ�AB�Ĳ���C
     }

    if(node->nodeVal=='|')
        node->firstpos=C;
    if(node->nodeVal=='@'){
        if((node->left)->nullable) //���������ɿ�
            node->firstpos=C;
        else node->firstpos=A;
    }
   }
}

int note_lastpos(STnode *node)
{
    if(!isOpt(node->nodeVal)|node->nodeVal=='#')
   {
       node->lastpos.push_back(node->nodeId);//��Ϊ�ַ�ֱ�Ӽ����Լ�
       return 0;
   }
   else{
        if(node->nodeVal=='*')
        {
            node->lastpos=(node->left)->lastpos;//��Ϊ* ͬ������
            return 0; //�����൱��break����Ȼ����ִ������Ĵ���ᱨ����Ϊ*û��������
        }

        vector<int> A=(node->left)->lastpos;
        vector<int> B=(node->right)->lastpos;
        vector<int> C;
        C=A;
     for(int i=0;i<B.size();i++)
     {   int flag;
         for(int j=0;j<A.size();j++)
         {
             if(A[j]==B[i]) flag=0; //�ҵ���ͬ�����0 �����
             else flag=1;
         }
         if(flag) C.push_back(B[i]); //����ͬ�ļ��룬�õ�AB�Ĳ���C
     }

    if(node->nodeVal=='|')
        node->lastpos=C;
    if(node->nodeVal=='@'){
        if((node->right)->nullable) //���������ɿ�
            node->lastpos=C;
        else node->lastpos=B;
    }
   }
}
STnode *change=NULL;
int find_pos(int num,STnode *node) //�ҵ�id��Ӧ�Ľ�㣬��change��� ֻ��Ҷ�ӽڵ���followpos
{
    if(node!=NULL)
    {
        find_pos(num,node->left);
        find_pos(num,node->right);
        if(node->left==NULL&&node->right==NULL)
        {
            if(node->nodeId==num)
            {  change=node;
                  return 0;}
        }
    }
}
int note_followpos(STnode *node,STnode *head)
{
    if(node->nodeVal=='@')
    {
        vector<int> A=(node->right)->firstpos;
        vector<int> B=(node->left)->lastpos;
        //for(int j=0;j<A.size();j++)
           // cout<<A[j];

        for(int i=0;i<B.size();i++)
        {
            find_pos(B[i],head);
            for(int j=0;j<A.size();j++)
            {   int flag=1;
                for(int k=0;k<change->followpos.size();k++)
                {  if(change->followpos[k]==A[j])//�Ѿ��еĽڵ㲻����
                    {  flag=0;break; }}
                if (flag)
                change->followpos.push_back(A[j]); //��˳��ѹ�룬ֱ�ӵ��ھ͸�����ǰ���
            }
        }
    }
    else if(node->nodeVal=='*')
    {
        vector<int> A=node->firstpos;
        vector<int> B=node->lastpos;
        for(int i=0;i<B.size();i++)
        {
            find_pos(B[i],head);
            for(int j=0;j<A.size();j++)
            {
                int flag=1;
                for(int k=0;k<change->followpos.size();k++)
                {  if(change->followpos[k]==A[j])//�Ѿ��еĽڵ㲻����
                    {  flag=0;break; }}
                if (flag)
                change->followpos.push_back(A[j]); //��˳��ѹ�룬ֱ�ӵ��ھ͸�����ǰ���
            }
        }
    }
    else return 0;
}

int post_order(STnode *node,STnode *head)//������������������ֵ
{
    if(node!=NULL)
    {
        if(post_order(node->left,head))
        {
            post_order(node->right,head);
            note_nullable(node);
            note_firstpos(node);
            note_lastpos(node);
            note_followpos(node,head);
        }
    }
    return 1;
}

/***************************************���********************************************/
int print_node(STnode *node) //���������ֵ
{
    cout<<node->nodeId<<' '<<node->nodeVal<<endl;
            if(!node->nullable) cout<<"not null"<<endl;
            else cout<<"null"<<endl;
    vector<int> A=node->firstpos;
    vector<int> B=node->lastpos;
    vector<int> C=node->followpos;
    cout<<"firstpos:";
    for(int i=0;i<A.size();i++)
        cout<<A[i]<<" ";
        cout<<endl;
      cout<<"lastpos:";
    for(int i=0;i<B.size();i++)
        cout<<B[i]<<" ";
        cout<<endl;
      cout<<"followpos:";
    for(int i=0;i<C.size();i++)
        cout<<C[i]<<" ";
        cout<<endl;
    cout<<"~~~~~~~~~~~~~~~~~"<<endl;
}

int print_all(STnode *node) //������н��
{
     if(node!=NULL)
    {
        if(print_all(node->left))
        {
            print_all(node->right);
            print_node(node);
        }
    }
    return 1;
}

/***********************************���첢���NFA***************************************/
queue <char> poland;
int polandexp(STnode *node)
{
    if(node!=NULL)
    {
        if(polandexp(node->left))
        {
            polandexp(node->right);
            poland.push(node->nodeVal);
        }
    }
    return 1;
}

typedef struct trans{
    int target;
    char val; //%��ʾ�շ���
}trans;
typedef struct state{
    int id;
    int type; //1��ʼ��0��ͨ��-1����
    vector <trans> next;
}state;
vector <state> save; //��save��������е�state��state����trans�䵱�����ָ��
stack <int> headtail; //ֻ��ÿ��NFAͷβ��id���Ƚ�head���ȳ�tail
int NFA()
{
    int num=0;
    while(poland.size()!=2) //�������������Ԫ�� һ��@һ��# ͬʱ�䵱ջ������
    {
        char val=poland.front();
        if(isOpt(val)){
            if(val=='@'){
               int head1,head2,tail1,tail2;
               tail2=headtail.top(); headtail.pop();
               head2=headtail.top(); headtail.pop();
               tail1=headtail.top(); headtail.pop();
               head1=headtail.top(); headtail.pop();
               trans tmp;
               tmp.target=save[head2].id;
               tmp.val='%';
               save[tail1].next.push_back(tmp);
               save[tail1].type=0;
               save[head2].type=0;//�ı䱻�ϲ��Ľ��״̬
               headtail.push(head1); headtail.push(tail2);
            }
            else if(val=='|'){
                state head,tail;
                head.id=num;num++; head.type=1; //head��id����state�Ľű���ͬ
                tail.id=num;num++; tail.type=-1;  //�½�����״̬
                int head1,head2,tail1,tail2;
                tail2=headtail.top(); headtail.pop();
                head2=headtail.top(); headtail.pop();
                tail1=headtail.top(); headtail.pop();
                head1=headtail.top(); headtail.pop();
                trans tmp;//��������trans
                tmp.target=save[head1].id; tmp.val='%';
                head.next.push_back(tmp);
                tmp.target=save[head2].id; tmp.val='%';
                head.next.push_back(tmp);
                tmp.target=tail.id; tmp.val='%';
                save[tail1].next.push_back(tmp);
                save[tail2].next.push_back(tmp);
                save.push_back(head); save.push_back(tail);//�������µĽ��
                save[head1].type=0;save[tail1].type=0;save[head2].type=0;save[tail2].type=0;//�ı�״̬
                headtail.push(head.id); headtail.push(tail.id); //����NFA����βѹ��
            }
            else if(val=='*'){
                state head,tail;
                head.id=num;num++; head.type=1;
                tail.id=num;num++; tail.type=-1;
                int head1,tail1;
                tail1=headtail.top(); headtail.pop();
                head1=headtail.top(); headtail.pop();
                trans tmp;
                tmp.target=save[head1].id; tmp.val='%';
                head.next.push_back(tmp);
                tmp.target=tail.id; tmp.val='%';
                save[tail1].next.push_back(tmp);
                tmp.target=tail.id; tmp.val='%';
                head.next.push_back(tmp);
                tmp.target=save[head1].id; tmp.val='%';
                save[tail1].next.push_back(tmp);
                save.push_back(head); save.push_back(tail);//�洢�½��
                save[head1].type=0;save[tail1].type=0;//�ı�״̬
                headtail.push(head.id); headtail.push(tail.id);
            }
        }
        else {
            state A; A.id=num; num++; A.type=1;
            state B; B.id=num; num++; B.type=-1;
            trans tmp; tmp.target=B.id; tmp.val=val;
            A.next.push_back(tmp);
            save.push_back(A);  save.push_back(B);
            headtail.push(A.id); headtail.push(B.id);
        }
    poland.pop();
    }
}

int print_NFA()
{
    for(int i=0;i<save.size();i++)
    {
        //cout<<save[i].id<<' '<<save[i].next[0].target<<" "<<save[i].next[0].val<<endl;

       cout<<save[i].id<<':';
       for(int j=0;j<save[i].next.size();j++)
        {
            cout<<save[i].next[j].val<<' '<<save[i].next[j].target<<' ';
        }
        cout<<endl;
    }
}
/******************************����DFA*******************************/
typedef struct line{
    int id;
    char letter;
}line;

typedef struct Dnode{
    int Id;
    vector<int> collect; //�������Ԫ��
    int mark;
    vector<line> followpos;//���µ�Id����ĸ����
}Dnode;
vector<Dnode> Dstates;

vector<int> temp;//����Dtran�Ľ��
int Dtran(STnode *root,int x, char c)
{
     if(root!=NULL)
    {
        Dtran(root->left,x,c);
        Dtran(root->right,x,c); //�ݹ���������
        if(root->left==NULL&&root->right==NULL)//�����Ҷ�ӽڵ�
        {   int yes=0;
            vector<int> s=Dstates[x].collect;
            for(int k=0;k<s.size();k++)
            {
                if(root->nodeId==s[k]) yes=1; //����ýڵ������ڼ����˵������Ҫ�ҵĽڵ�
            }
            if(root->nodeVal==c&&yes) //��Ҫ�ҵĽ����ҷ��Ŷ�Ӧ���򽫸ýڵ��follow����
            {
                vector<int> A=root->followpos;
               // if(A.empty()) return 0; //����÷�����# û��followpos return0ֱ���������ټ���ִ��
                if(temp.empty()) temp=A;
                else
                {
                  for(int i=0;i<A.size();i++)
                    { int flag;
                      for(int j=0;j<temp.size();j++)
                      {
                        if(A[i]==temp[j]) flag=0; //�ҵ���ͬ�����0 �����
                        else flag=1;
                      }
                      if(flag) temp.push_back(A[i]); //����ͬ�ļ��룬�õ�����
                    }
                 }
                 sort(temp.begin(),temp.end());
                 }}}
    return 0;
}

int instate()
{
    int flag=0;
    sort(temp.begin(),temp.end());
    for(int i=0;i<Dstates.size();i++)
    {
        if(Dstates[i].collect==temp)  //��˳��
        {
            flag=i; return flag;
        }

    }
   return -1;
}
int DFA(STnode *root,string check)
{
   int number=1;
   Dnode node;
   node.Id=number;
   node.collect=root->firstpos;
   node.mark=0;
   Dstates.push_back(node);//��ʼ��Dstates��ֻ��һ��δ��ǵ�node
   int j=0;//������¼Mark��������
   int unmarked=1;
   while(unmarked)//�������δ��ǵ�
   {
      Dstates[j].mark=1; //���
      for(int i=0;i<check.size();i++)
      {
          vector<int> U;
         // cout<<check[i]<<"$";
          Dtran(root,j,check[i]);//�����﷨���ĸ�����ǰ״̬��λ�ú���Ҫ���ҵ��ַ�
          U=temp;//�õ�followpos����

          cout<<"U:";
          for(int k=0;k<U.size();k++)
          cout<<U[k]<<' ';
          cout<<endl;

      if(!U.empty()) {
         int res=instate();
          if(res==-1) //�Ƚ�U��Dstates���ֲ�ͬ
          {
             number++;
             Dnode newnode;
             newnode.Id=number;
             newnode.collect=U;
             newnode.mark=0;
             Dstates.push_back(newnode); //��Ϊδ���״̬����Dstates
             line B;
             B.id=newnode.Id; B.letter=check[i];
             Dstates[j].followpos.push_back(B);
             cout<<"jiaru"<<endl;
          }
          else{  //״̬��ͬ���򽫸�״̬����֮ǰ��followpos
          line A;
          A.id=Dstates[res].Id; A.letter=check[i];
          Dstates[j].followpos.push_back(A);
          }
          }
          temp.clear(); U.clear();
      }
       unmarked=0;//���� �����Ƿ���δ��ǵ�
       for(int i=0;i<Dstates.size();i++)
       {
           if(Dstates[i].mark==0)
           {
               unmarked=1; j=i; //˵����δ��ǵ� j��¼δ��ǵ�λ��
           }

       }
   }
}

/***************************���DFA***************************/
int print_DFA()
{
    for(int i=0;i<Dstates.size();i++)
    {
        cout<<"״̬��"<<Dstates[i].Id<<endl;
        vector<int> A=Dstates[i].collect;
        for(int j=0;j<A.size();j++)
            cout<<A[j]<<' ';
        cout<<endl;
        vector<line> B=Dstates[i].followpos;
        for(int j=0;j<B.size();j++)
            cout<<B[j].id<<' '<<B[j].letter<<' ';
        cout<<endl;


    }
}
/*****************************��С��******************************/
int leaf_number=0;
int count_leaf(STnode *node) //���Ҷ�ӽڵ�
{
    if(node!=NULL)
    {
        count_leaf(node->left);
        count_leaf(node->right);
        if(node->left==NULL&&node->right==NULL)
        {
            leaf_number++;
        }
    }
    return 1;
}
vector<int> group;
vector<int> group_size;
vector<int> isend;
int count_group=0;
int is_end()
{
    group_size.push_back(0);
     for(int i=0;i<Dstates.size();i++)
    {   group.push_back(1); //��ʼ����ȫΪ״̬1
        isend.push_back(0);
       for(int j=0;j<Dstates[i].collect.size();j++)
       {
           if(Dstates[i].collect[j]==leaf_number)
           {
               group[i]=0;//���Խ���������Ϊ0���������ε���
               isend[i]=1;
               group_size[0]++; //���Ϊ0����������
           }
       }
    }
   group_size.push_back(Dstates.size() - group_size[0]);//һ��ʼֻ��0��1������
   count_group++;//��ű༭��1

   cout<<"group��ţ�"<<endl;
   for(int i=0;i<group.size();i++)
    cout<<group[i]<<' ';
   cout<<endl;
   return 0;
}

typedef struct DFA_node{
    vector<int> groupnum;
}DFA_node;

vector<DFA_node> min_DFA;//�䵱һ���䳤�Ķ�ά����,������Ϊ�ַ���������˳��������Ϊԭ״̬�����
                            //ÿ��Ϊһ��״̬
int have_diff(int gnum, int gsize)
{
    vector<int> same;
    for(int i=0;i<group.size();i++)
    {
        if(group[i]==gnum){
            same.push_back(i); cout<<i<<'+';
        }

    }

    //cout<<"have"<<same.size()<<gnum<<' '<<gsize<<endl;
    for(int i=0;i<gsize;i++)
    {
        for(int j=0;j<gsize;j++)
        {
            if(min_DFA[same[i]].groupnum!=min_DFA[same[j]].groupnum)
                return same[i];//�������ڲ�ͬ��״̬��
        }

    }
    return -1;
}//������Ż�-1

int have_same(int gnum, int gsize,int add,int count_group)
{
    vector<int> same;
    for(int i=0;i<group.size();i++)//��������group �ҳ�һ���
        if(group[i]==gnum)
            same.push_back(i);
    int count=0;
    for(int i=0;i<gsize;i++)
    {
        if(min_DFA[same[i]].groupnum==min_DFA[add].groupnum)
        {
            group[same[i]]=count_group;count++;
        }
    }
    group[add]=count_group;
    return count;
}

vector<int> res;//��������
vector<int> resg;//���������
int minimize(string check)
{
    for(int i=0;i<Dstates.size();i++){
        DFA_node A;
        for(int j=0;j<check.size();j++){
            A.groupnum.push_back(-1);
        }
        min_DFA.push_back(A);
        A.groupnum.clear();
    }//��ʼ��

    for(int i=0;i<Dstates.size();i++)//����ÿ�е�ֵ,ԭ״̬�����Ϊ�ű� �ҵ�group�ﻮ�ֺ����ڵڼ���
    {
        for(int j=0;j<Dstates[i].followpos.size();j++) //DFAÿ������ֻ��Ӧһ��ָ��
        {
            int index=check.find(Dstates[i].followpos[j].letter,0);
            int tmp=Dstates[i].followpos[j].id-1;
            min_DFA[i].groupnum[index]=group[tmp];
            //cout<<"tmp:"<<tmp<<"newtmp:"<<group[tmp]<<endl;
        }
    }//����min_DFA
    cout<<"��һ�θ��º�"<<endl;
    for(int i=0;i<min_DFA.size();i++)
    {
        for(int j=0;j<min_DFA[i].groupnum.size();j++)
            cout<< min_DFA[i].groupnum[j];
        cout<<endl;
    }
cout<<"��һ��group_size:";
for(int i=0;i<group_size.size();i++)
    cout<<group_size[i]<<' ';
   cout<<endl;

/*�Ӵ˴�i��ʾ���*/
for(int i=0;i<group_size.size(); )
   { cout<<"here?"<<endl;
    if(group_size[i]==1||group_size[i]==0)
        i++;
    else{
        int add=have_diff(i,group_size[i]);//������ź���Ĵ�С,addΪ��ͬ�����
        cout<<"����"<<i<<"�飬��ͬ�����Ϊ"<<add<<endl;
        if(add!=-1)//�ҵ��˲�ͬ
        {   int count;
            count_group++;//����һ����
            count=have_same(i,group_size[i],add,count_group);

            group_size.push_back(count);
            group_size[i]=group_size[i]-count; //��������״̬����

            cout<<"group��ţ�"<<endl;
            for(int j=0;j<group.size();j++)
                cout<<group[j]<<' ';
            cout<<endl;

            for(int j=0;j<Dstates.size();j++)
            {
                for(int k=0;k<Dstates[j].followpos.size();k++)
                {
                    int index=check.find(Dstates[j].followpos[k].letter,0);
                    int tmp=Dstates[j].followpos[k].id-1;
                    min_DFA[j].groupnum[index]=group[tmp];

                }
            }
            cout<<"���º�"<<endl;
            for(int i=0;i<min_DFA.size();i++)
            {   for(int j=0;j<min_DFA[i].groupnum.size();j++)
                    cout<< min_DFA[i].groupnum[j];
                cout<<endl;
            }
            cout<<"group_size:";
                        for(int i=0;i<group_size.size();i++)
                            cout<<group_size[i]<<' ';
                        cout<<endl;
        }

        else{//add=-1��û�в�ͬ

           /* for(int l=0;l<group.size();l++)//ѡÿһ��ĵ�һ����Ϊ���ս��,res��¼���
                if(group[l]==i)
                    res.push_back(l);
            cout<<"������"<<res.back()+1<<endl;*/
            i++;
        }
    }
 }
 cout<<"over~"<<endl;
 //����res
int x=0;
while(x<group_size.size())
{
    if(group_size[x]!=0){
        for(int m=0;m<group.size();m++)
        {
            if(group[m]==x)
            {
            res.push_back(m);
            resg.push_back(x);
            x++;
            break;
            }
        }
    }
    else  x++;
}
return 0;
}

void print_min(string check)
{
    for(int i=0;i<res.size();i++)
    {   cout<<"״̬:"<<resg[i]<<endl;
        vector<int> A=min_DFA[res[i]].groupnum;
        for(int j=0;j<A.size();j++)
            cout<<check[j]<<','<<A[j]<<endl;
    }
}
/**************************��ͼ*****************************/
int draw_NFA()
{
    ofstream OutFile;
    OutFile.open("NFA.dot");
    OutFile<<"digraph NFA{\n";
    OutFile<<save[0].id<<"[ style = filled, color=lightgrey ];\n";
    //cout<<save.size()<<endl;
    OutFile<<save[save.size()-1].id<<"[ shape = doublecircle ];\n";
    for(int i=0;i<save.size();i++)
    {
       for(int j=0;j<save[i].next.size();j++)
        {
            OutFile<<save[i].id<<"->";
            OutFile<<save[i].next[j].target<<" [label= \""<<save[i].next[j].val<<"\" ];\n";
        }
    }
    OutFile<<"}";
    OutFile.close();
}

int draw_min(string check)
{
    ofstream OutFile;
    OutFile.open("pic.dot");
    OutFile<<"digraph pic{\n";
    OutFile<<group[0]<<"[ style = filled, color=lightgrey ];\n";
    for(int i=0;i<isend.size();i++)
        if(isend[i]==1)
            OutFile<<group[i]<<"[ shape = doublecircle ];\n";
    for(int i=0;i<res.size();i++){
        vector<int> A=min_DFA[res[i]].groupnum;
        for(int j=0;j<A.size();j++)
        {
            if(A[j]!=-1){
            OutFile<<resg[i]<<"->";
            OutFile<<A[j]<<" [label= \""<<check[j]<<"\" ];\n";
            }
        }
    }
    OutFile<<"}";
    OutFile.close();
}

int draw_DFA()
{
    ofstream OutFile;
    OutFile.open("DFA.dot");
    OutFile<<"digraph DFA{\n";
    OutFile<<Dstates[0].Id<<"[ style = filled, color=lightgrey ];\n";
    for(int i=0;i<Dstates.size();i++)
    {
        for(int j=0;j<Dstates[i].collect.size();j++)
        {
            if(Dstates[i].collect[j]==leaf_number)
            {
                OutFile<<Dstates[i].Id<<"[ shape = doublecircle ];\n";
            }
        }
    }
    for(int i=0;i<Dstates.size();i++)
    {
        vector<line> B=Dstates[i].followpos;
        for(int j=0;j<B.size();j++){
            OutFile<<Dstates[i].Id<<"->";
            OutFile<<B[j].id<<" [label= \""<<B[j].letter<<"\" ];\n";
        }
    }
    OutFile<<"}";
    OutFile.close();
}
/************************������****************************/
int main()
{
   string input,check;
   string mult="@#";
   cin>>input;
   int len=input.size();
   for(int i=0;i<len;i++){
       if(isalpha(input[i])&&isalpha(input[i+1])){
           input.insert(i+1,mult,0,1);
           len++;}
       else if(input[i]==')'&&isalpha(input[i+1])){
           input.insert(i+1,mult,0,1);
           len++;}
       else if(input[i]=='*'&&isalpha(input[i+1])){
           input.insert(i+1,mult,0,1);
           len++;}
       else if(input[i]=='*'&&input[i+1]=='('){
           input.insert(i+1,mult,0,1);
           len++;}
       else if(isalpha(input[i])&&input[i+1]=='('){
           input.insert(i+1,mult,0,1);
           len++;}
   }   //���ַ�������Ԥ������ӳ˺�
   for(int i=0;i<len;i++)
    cout<<input[i];
    cout<<endl;

    for(int i=0;i<len;i++) //check��������������ַ����
    {   int flag=1;
        for(int j=0;j<check.size();j++)
        {
            if(!isOpt(input[i])&&input[i]==check[j])
                flag=0;
        }
        if(flag&&!isOpt(input[i]))
            check.push_back(input[i]);
    }
 /*for(int i=0;i<check.size();i++)
    cout<<check[i];
    cout<<endl;*/

   STnode root;
   STnode *node,*head; //node�ɱ䣬head���ɱ�
   root=syntexTree(input); //rootΪ���ڵ㣬����
   node=&root;
   /* while(node!=NULL)
    {
        cout<<node->nodeVal<<endl;
        node=node->left;
    }
    cout<<"------------"<<endl;*/
    node=&root; //��λ
    num(node);//��Ҷ�ӽڵ㸳ֵ

    node=&root; head=node; post_order(node,head);//��������ֵ
    node=&root; print_all(node); //���

    node=&root;
    polandexp(node);
    NFA();
    cout<<"NFA:"<<endl;
    print_NFA();
    node=&root;
    DFA(node,check);
    cout<<"--------------"<<endl;
    cout<<"DFA:"<<endl;
    print_DFA();
    if(Dstates.size()==1)
    {
        cout<<"min DFA:"<<endl;
        print_DFA();
    }
    else{
    node=&root;
    count_leaf(node);
    is_end();
    minimize(check);
    print_min(check);
    }
    draw_NFA();
    draw_DFA();
    draw_min(check);
    return 0;
}

