#include <iostream>
#include <stack>
#include <vector>
#include<algorithm>
#include <queue>
#include<fstream>
using namespace std;

typedef struct STnode {     //定义语法树的结点
char nodeVal; //结点的值
int nodeId;  //唯一标识叶子节点
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
/*****************************构建语法树***********************************/
int isOpt(char x)
{
    if(x>=65&&x<=90)
        return 0;
    else if (x>=97&&x<=122) //大写和小写
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
    if(x==0&&y==1) return 0;//退出两个
    if(x==0||y==0) return -1;
    else if(x>=y) return 1;//优先级高
    else return -1;//优先级低，压

}

STnode syntexTree(string input)
{
    int len=input.length();
    for(int i=0;i<len;i++)
    {
        if(!isOpt(input[i])) //如果不是运算符
        {
            STnode alpNode;
            alpNode.nodeVal=input[i];
            alpNode.left=NULL;
            alpNode.right=NULL;  //将字母转化为结点
            Alp.push(alpNode); //压入结点栈
        }
        else
        {
            STnode optNode;
            optNode.nodeVal=input[i];
            optNode.nodeId=-1;
            optNode.left=NULL;
            optNode.right=NULL;  //将运算符转化为结点
            if(Opt.empty())
                 Opt.push(optNode); //为空直接加入
            else{
                STnode old=Opt.top();
                int res=compare(old,optNode);//取栈顶运算符和新符号比较
                while(res==1)  //栈顶结点优先级高
                {   Opt.pop(); //处理栈顶结点
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
                    old.left= new STnode (letter2);//后进入的先弹出，所以要反过来
                    old.right= new STnode (letter1); //这里必须用new赋值，开辟新的内存来存
    // old.left=&letter2; old.right=&letter1; 如果用这种赋值方式，相当于没有新增地址，而是一直在原地址上更新值，所以会出现无限循环
                        Alp.push(old);
                        }
                    if(Opt.empty()) {Opt.push(optNode); break;}
                    else{
                        old=Opt.top();
                        res=compare(old,optNode); //一直比较直到老结点优先级变低
                    }

                }
                if(res==-1)
                Opt.push(optNode);//新结点优先级高，压入
                else if(res==0){
                    STnode test=Opt.top();
                    Opt.pop(); //弹出左括号
                }
            }

        }
    }
        STnode A=Alp.top(); Alp.pop();  //最后将表达式和结束符#用@链接
        STnode B=Opt.top(); Opt.pop();
        STnode optNode;
        optNode.nodeVal='@';
        optNode.nodeId=-2; //代表根节点
        optNode.left=NULL;
        optNode.right=NULL;
        optNode.left=new STnode (A);  optNode.right=new STnode (B);
        STnode head=optNode;
        return head;//返回根节点
}

/*****************************初始化，为叶子增加标号*************************************/
int notenum=1;
void num(STnode *node) //给叶子节点添加标号
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

/**************************后序遍历计算三组值，nullable,firstpos,lastpos*************************************/
int note_nullable(STnode *node)  //必须用指针类型，如果直接传入节点 无法保留改变后的值
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
       node->firstpos.push_back(node->nodeId);//若为字符直接加入自己
       return 0;
   }
   else{
        if(node->nodeVal=='*')
        {
            node->firstpos=(node->left)->firstpos;//若为* 同左子树
            return 0; //这里相当于break，不然继续执行下面的代码会报错，因为*没有右子树
        }

        vector<int> A=(node->left)->firstpos;
        vector<int> B=(node->right)->firstpos;
        vector<int> C;
        C=A;
     for(int i=0;i<B.size();i++)
     {   int flag;
         for(int j=0;j<A.size();j++)
         {
             if(A[j]==B[i]) flag=0; //找到相同的输出0 不添加
             else flag=1;
         }
         if(flag) C.push_back(B[i]); //将不同的加入，得到AB的并集C
     }

    if(node->nodeVal=='|')
        node->firstpos=C;
    if(node->nodeVal=='@'){
        if((node->left)->nullable) //若左子树可空
            node->firstpos=C;
        else node->firstpos=A;
    }
   }
}

int note_lastpos(STnode *node)
{
    if(!isOpt(node->nodeVal)|node->nodeVal=='#')
   {
       node->lastpos.push_back(node->nodeId);//若为字符直接加入自己
       return 0;
   }
   else{
        if(node->nodeVal=='*')
        {
            node->lastpos=(node->left)->lastpos;//若为* 同左子树
            return 0; //这里相当于break，不然继续执行下面的代码会报错，因为*没有右子树
        }

        vector<int> A=(node->left)->lastpos;
        vector<int> B=(node->right)->lastpos;
        vector<int> C;
        C=A;
     for(int i=0;i<B.size();i++)
     {   int flag;
         for(int j=0;j<A.size();j++)
         {
             if(A[j]==B[i]) flag=0; //找到相同的输出0 不添加
             else flag=1;
         }
         if(flag) C.push_back(B[i]); //将不同的加入，得到AB的并集C
     }

    if(node->nodeVal=='|')
        node->lastpos=C;
    if(node->nodeVal=='@'){
        if((node->right)->nullable) //若左子树可空
            node->lastpos=C;
        else node->lastpos=B;
    }
   }
}
STnode *change=NULL;
int find_pos(int num,STnode *node) //找到id对应的结点，用change标记 只有叶子节点有followpos
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
                {  if(change->followpos[k]==A[j])//已经有的节点不加入
                    {  flag=0;break; }}
                if (flag)
                change->followpos.push_back(A[j]); //按顺序压入，直接等于就覆盖了前面的
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
                {  if(change->followpos[k]==A[j])//已经有的节点不加入
                    {  flag=0;break; }}
                if (flag)
                change->followpos.push_back(A[j]); //按顺序压入，直接等于就覆盖了前面的
            }
        }
    }
    else return 0;
}

int post_order(STnode *node,STnode *head)//后序遍历，计算出三个值
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

/***************************************输出********************************************/
int print_node(STnode *node) //输出结点相关值
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

int print_all(STnode *node) //输出所有结点
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

/***********************************构造并输出NFA***************************************/
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
    char val; //%表示空符号
}trans;
typedef struct state{
    int id;
    int type; //1开始，0普通，-1结束
    vector <trans> next;
}state;
vector <state> save; //用save来存放所有的state，state里面trans充当链表的指针
stack <int> headtail; //只存每个NFA头尾的id，先进head，先出tail
int NFA()
{
    int num=0;
    while(poland.size()!=2) //处理最后多的两个元素 一个@一个# 同时充当栈的作用
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
               save[head2].type=0;//改变被合并的结点状态
               headtail.push(head1); headtail.push(tail2);
            }
            else if(val=='|'){
                state head,tail;
                head.id=num;num++; head.type=1; //head的id和他state的脚标相同
                tail.id=num;num++; tail.type=-1;  //新建两个状态
                int head1,head2,tail1,tail2;
                tail2=headtail.top(); headtail.pop();
                head2=headtail.top(); headtail.pop();
                tail1=headtail.top(); headtail.pop();
                head1=headtail.top(); headtail.pop();
                trans tmp;//建立四条trans
                tmp.target=save[head1].id; tmp.val='%';
                head.next.push_back(tmp);
                tmp.target=save[head2].id; tmp.val='%';
                head.next.push_back(tmp);
                tmp.target=tail.id; tmp.val='%';
                save[tail1].next.push_back(tmp);
                save[tail2].next.push_back(tmp);
                save.push_back(head); save.push_back(tail);//存两个新的结点
                save[head1].type=0;save[tail1].type=0;save[head2].type=0;save[tail2].type=0;//改变状态
                headtail.push(head.id); headtail.push(tail.id); //将新NFA的首尾压入
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
                save.push_back(head); save.push_back(tail);//存储新结点
                save[head1].type=0;save[tail1].type=0;//改变状态
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
/******************************生成DFA*******************************/
typedef struct line{
    int id;
    char letter;
}line;

typedef struct Dnode{
    int Id;
    vector<int> collect; //集合里的元素
    int mark;
    vector<line> followpos;//用新的Id加字母区分
}Dnode;
vector<Dnode> Dstates;

vector<int> temp;//传递Dtran的结果
int Dtran(STnode *root,int x, char c)
{
     if(root!=NULL)
    {
        Dtran(root->left,x,c);
        Dtran(root->right,x,c); //递归遍历结点树
        if(root->left==NULL&&root->right==NULL)//如果是叶子节点
        {   int yes=0;
            vector<int> s=Dstates[x].collect;
            for(int k=0;k<s.size();k++)
            {
                if(root->nodeId==s[k]) yes=1; //如果该节点的序号在集合里，说明是需要找的节点
            }
            if(root->nodeVal==c&&yes) //是要找的结点而且符号对应，则将该节点的follow并入
            {
                vector<int> A=root->followpos;
               // if(A.empty()) return 0; //如果该符号是# 没有followpos return0直接跳出不再继续执行
                if(temp.empty()) temp=A;
                else
                {
                  for(int i=0;i<A.size();i++)
                    { int flag;
                      for(int j=0;j<temp.size();j++)
                      {
                        if(A[i]==temp[j]) flag=0; //找到相同的输出0 不添加
                        else flag=1;
                      }
                      if(flag) temp.push_back(A[i]); //将不同的加入，得到并集
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
        if(Dstates[i].collect==temp)  //有顺序
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
   Dstates.push_back(node);//初始化Dstates，只含一个未标记的node
   int j=0;//用来记录Mark到了哪里
   int unmarked=1;
   while(unmarked)//如果还有未标记的
   {
      Dstates[j].mark=1; //标记
      for(int i=0;i<check.size();i++)
      {
          vector<int> U;
         // cout<<check[i]<<"$";
          Dtran(root,j,check[i]);//传入语法树的根，当前状态的位置和需要查找的字符
          U=temp;//得到followpos集合

          cout<<"U:";
          for(int k=0;k<U.size();k++)
          cout<<U[k]<<' ';
          cout<<endl;

      if(!U.empty()) {
         int res=instate();
          if(res==-1) //比较U和Dstates发现不同
          {
             number++;
             Dnode newnode;
             newnode.Id=number;
             newnode.collect=U;
             newnode.mark=0;
             Dstates.push_back(newnode); //作为未标记状态加入Dstates
             line B;
             B.id=newnode.Id; B.letter=check[i];
             Dstates[j].followpos.push_back(B);
             cout<<"jiaru"<<endl;
          }
          else{  //状态相同，则将该状态加入之前的followpos
          line A;
          A.id=Dstates[res].Id; A.letter=check[i];
          Dstates[j].followpos.push_back(A);
          }
          }
          temp.clear(); U.clear();
      }
       unmarked=0;//清零 检验是否有未标记的
       for(int i=0;i<Dstates.size();i++)
       {
           if(Dstates[i].mark==0)
           {
               unmarked=1; j=i; //说明有未标记的 j记录未标记的位置
           }

       }
   }
}

/***************************输出DFA***************************/
int print_DFA()
{
    for(int i=0;i<Dstates.size();i++)
    {
        cout<<"状态："<<Dstates[i].Id<<endl;
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
/*****************************最小化******************************/
int leaf_number=0;
int count_leaf(STnode *node) //输出叶子节点
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
    {   group.push_back(1); //初始化，全为状态1
        isend.push_back(0);
       for(int j=0;j<Dstates[i].collect.size();j++)
       {
           if(Dstates[i].collect[j]==leaf_number)
           {
               group[i]=0;//可以结束的组别归为0，后面依次递增
               isend[i]=1;
               group_size[0]++; //组号为0的数量增加
           }
       }
    }
   group_size.push_back(Dstates.size() - group_size[0]);//一开始只有0和1两个组
   count_group++;//组号编辑到1

   cout<<"group编号："<<endl;
   for(int i=0;i<group.size();i++)
    cout<<group[i]<<' ';
   cout<<endl;
   return 0;
}

typedef struct DFA_node{
    vector<int> groupnum;
}DFA_node;

vector<DFA_node> min_DFA;//充当一个变长的二维数组,横坐标为字符，按输入顺序，纵坐标为原状态的序号
                            //每行为一个状态
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
                return same[i];//返回组内不同的状态号
        }

    }
    return -1;
}//返回组号或-1

int have_same(int gnum, int gsize,int add,int count_group)
{
    vector<int> same;
    for(int i=0;i<group.size();i++)//遍历所有group 找出一组的
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

vector<int> res;//来存放序号
vector<int> resg;//用来存组号
int minimize(string check)
{
    for(int i=0;i<Dstates.size();i++){
        DFA_node A;
        for(int j=0;j<check.size();j++){
            A.groupnum.push_back(-1);
        }
        min_DFA.push_back(A);
        A.groupnum.clear();
    }//初始化

    for(int i=0;i<Dstates.size();i++)//更新每行的值,原状态序号作为脚标 找到group里划分后属于第几类
    {
        for(int j=0;j<Dstates[i].followpos.size();j++) //DFA每个输入只对应一个指向
        {
            int index=check.find(Dstates[i].followpos[j].letter,0);
            int tmp=Dstates[i].followpos[j].id-1;
            min_DFA[i].groupnum[index]=group[tmp];
            //cout<<"tmp:"<<tmp<<"newtmp:"<<group[tmp]<<endl;
        }
    }//更新min_DFA
    cout<<"第一次更新后"<<endl;
    for(int i=0;i<min_DFA.size();i++)
    {
        for(int j=0;j<min_DFA[i].groupnum.size();j++)
            cout<< min_DFA[i].groupnum[j];
        cout<<endl;
    }
cout<<"第一次group_size:";
for(int i=0;i<group_size.size();i++)
    cout<<group_size[i]<<' ';
   cout<<endl;

/*从此处i表示组号*/
for(int i=0;i<group_size.size(); )
   { cout<<"here?"<<endl;
    if(group_size[i]==1||group_size[i]==0)
        i++;
    else{
        int add=have_diff(i,group_size[i]);//传入组号和组的大小,add为不同的组号
        cout<<"遍历"<<i<<"组，不同的序号为"<<add<<endl;
        if(add!=-1)//找到了不同
        {   int count;
            count_group++;//新增一个组
            count=have_same(i,group_size[i],add,count_group);

            group_size.push_back(count);
            group_size[i]=group_size[i]-count; //更新组内状态数量

            cout<<"group编号："<<endl;
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
            cout<<"更新后"<<endl;
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

        else{//add=-1，没有不同

           /* for(int l=0;l<group.size();l++)//选每一组的第一个作为最终结果,res记录序号
                if(group[l]==i)
                    res.push_back(l);
            cout<<"加入结果"<<res.back()+1<<endl;*/
            i++;
        }
    }
 }
 cout<<"over~"<<endl;
 //生成res
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
    {   cout<<"状态:"<<resg[i]<<endl;
        vector<int> A=min_DFA[res[i]].groupnum;
        for(int j=0;j<A.size();j++)
            cout<<check[j]<<','<<A[j]<<endl;
    }
}
/**************************画图*****************************/
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
/************************主函数****************************/
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
   }   //对字符串进行预处理，添加乘号
   for(int i=0;i<len;i++)
    cout<<input[i];
    cout<<endl;

    for(int i=0;i<len;i++) //check包含输入的所有字符类别
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
   STnode *node,*head; //node可变，head不可变
   root=syntexTree(input); //root为根节点，保留
   node=&root;
   /* while(node!=NULL)
    {
        cout<<node->nodeVal<<endl;
        node=node->left;
    }
    cout<<"------------"<<endl;*/
    node=&root; //定位
    num(node);//给叶子节点赋值

    node=&root; head=node; post_order(node,head);//计算三组值
    node=&root; print_all(node); //输出

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

