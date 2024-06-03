#include<iostream>
#include<string>
#include<vector>
#include<stack>

using namespace std;

int director=0;

//--------------------------------------------------------------------------------------------
//�ʷ�������
//ע��Դ����ÿ�����ʷ��ż�����ÿո�ָ�

FILE* fp;				//�ļ�ָ��

enum Type{
	TP_KW,
	TP_NUM,
	TP_OP,
	START
};

enum TokenCode
{
	/*δ����*/
	TK_UNDEF = 0,

	/* �ؼ��� */
	KW_WHILE,	
    KW_IF,
    KW_ELSE,
    KW_INT,

	/* ����� */
	TK_PLUS,	//+�Ӻ�
	TK_MINUS,	//-����
	TK_STAR,	//*�˺�
	TK_DIVIDE,	///����
    TK_MOD,     //ȡ��
	TK_ASSIGN,	//=��ֵ�����
	TK_EQ,		//==���ں�
    TK_NOTEQ,   //!=������
	TK_LT,		//<С�ں�
	TK_LEQ,		//<=С�ڵ��ں�
	TK_GT,		//>���ں�
	TK_GEQ,		//>=���ڵ��ں�
	TK_AND,		//&&
    TK_OR,		//||
    TK_NOT,		//!
    TK_DECR,    //--�Լ�
    TK_INCR,    //++����

	/* �ָ��� */
	TK_OPENPA,	//(��Բ����
	TK_CLOSEPA,	//)��Բ����
	TK_OPENBR,	//[��������
	TK_CLOSEBR,	//]��������
	TK_BEGIN,	//{�������
	TK_END,		//}�Ҵ�����
	TK_COMMA,	//,����
	TK_SEMOCOLOM,	//;�ֺ�
	
	/* ���� */
	TK_INT,		//���ͳ���

	/* ��ʶ�� */
	TK_IDENT,

	/*��ʼ��*/
	TK_START,
	//������
	TK_EOF
};

struct Word{
	int code;
	string value;
	string attribute;
};


TokenCode code = TK_UNDEF;		//��¼���ʵ��ֱ���
const int keyMAX = 4;	//�ؼ�������
const int opMAX = 25;
int row = 1;	//��������

string keyWord[keyMAX]={"while","if","else","int"};//��¼�ؼ���
string op[opMAX]={"+","-","*","/","%","=","==","!=",
"<","<=",">",">=","&&","||","!","--","++","(",")",
"[","]","{","}",",",";"};
//�ж��Ƿ�Ϊ�ؼ���
bool isKey(Word& w)
{
	string token=w.value;
	for (int i = 0; i < keyMAX; i++)
	{
		if (token.compare(keyWord[i]) == 0)
		{
			w.code=i+1;
			return true;
		}
			
	}
	w.code=TK_IDENT;
	return false;
}

//�ж��ַ��Ƿ�Ϊ��ĸ
bool isLetter(char letter)
{
	if ((letter >= 'a'&&letter <= 'z') || (letter >= 'A' &&letter <= 'Z'))
		return true;
	return false;

}

//�ж��ַ��Ƿ�Ϊ����
bool isDigit(char digit)
{
	if (digit >= '0'&&digit <= '9')
		return true;
	return false;
}

//��ʶ��ǰ����ĵ��ʣ����ڵ���
string curW;

Word lexicalAnalysis(string token,Type tp){
	Word w;
	w.value=token;
	curW=token;
	switch(tp){
		case TP_KW:{
			isKey(w);
			break;
		}
		case TP_NUM:{
			w.code=TK_INT;
			break;
		}
		case TP_OP:{
			for(int i=0;i<opMAX;i++){
				if(token.compare(op[i])==0){
					w.code=i+5;
					break;
				}
			}
			break;
		}
		case START:{
			w.code=TK_START;
			break;
		}

	}

	//cout<<w.value<<" "<<w.code<<" "<<w.attribute<<endl;
	return w;
	
}


bool isEOF=false;
void rollBack(Word& w) { // ����һ������ + ' '
    director--;
	if(!isEOF){
		string value = w.value;
		int len = value.length();

		// ���Ȼ���һ���ո�
		ungetc(' ', fp);

		// �Ӻ���ǰ����ÿ���ַ�
		for (int i = len - 1; i >= 0; i--) {
			ungetc(value[i], fp);
		}
	}else{
		ungetc(EOF,fp);
	}
}


Word getNextWord() {
    director++;
    Type tp;
    char ch;
    string token;


    while ((ch = static_cast<char>(fgetc(fp))) != EOF) {
        // ���Կո�Tab �ͻس�
        if (ch == ' ' || ch == '\t' || ch == '\n') {
            if (ch == '\n') {
                row++;  // �������з�����¼������ row �� 1
            }
            continue;  // ����ִ��ѭ��
        }
        
        // ����ؼ��ֻ��ʶ��
        if (isLetter(ch)) {
            token.clear();  // ��ʼ�� token
            do {
                token.push_back(ch);  // ����ȡ���ַ� ch ���� token ��
                ch = fgetc(fp);       // ��ȡ��һ���ַ�
            } while (isLetter(ch) || isDigit(ch));

            ungetc(ch, fp);  // �������һ����ȡ�ķ���ĸ�����ַ�
            tp = TP_KW;
            return lexicalAnalysis(token, tp);
        }
        
        // ������
        if (isDigit(ch)) {
            token.clear();  // ��ʼ�� token
            do {
                token.push_back(ch);  // ����ȡ���ַ� ch ���� token ��
                ch = fgetc(fp);       // ��ȡ��һ���ַ�
            } while (isDigit(ch));

            ungetc(ch, fp);  // �������һ����ȡ�ķ������ַ�
            tp = TP_NUM;
            return lexicalAnalysis(token, tp);
        }
        
        // ����ָ����������
        token.clear();
        token.push_back(ch);
		if(ch==EOF){
			isEOF=true;
			break;
		}else
        	ch = static_cast<char>(fgetc(fp));
		
        // ����˫�ַ������
        if (!isspace(ch) && ch != EOF) {
            // �ݴ��ַ����Է���������������
            token.push_back(ch);

            // ����Ƿ�Ϊ˫�ַ������
            if (token == "==" || token == "!=" || token == "<=" || token == ">=" || token == "&&" || token == "||") {
                // ȷ��˫�ַ������������Ҫ�����ַ�
            } else {
                // ����˫�ַ���������������һ���ַ�
                token.pop_back();
                ungetc(ch, fp);
            }
        } else if(ch==' ') {
            // ����ڶ����ַ��ǿհ��ַ��� EOF��ֱ�ӻ���
            ungetc(ch, fp);
        }

        // ���»����к�
        if (ch == '\n') {
            row++;
        }
        tp = TP_OP;
        return lexicalAnalysis(token, tp);
    }
	
	//��ǰch==EOF
	isEOF=true;
	Word w;
	w.value=';';
	w.code=TK_SEMOCOLOM;
	return w;  
}

//-----------------------------------------------------------------------------------------
//���������

//�����飬���ڴ��ݵ��ʷ������ԣ����ս��|�ս����
struct AttrGroup{
	string place;
	string inArray;
};

//��Ԫʽ
struct Quad {
	string op;
	string str1;
	string str2;
	string result;
};

vector<Quad>quads;

//�����ʱ�������
int tempVarNum=1;

//������Ԫʽ
void emit(string op,string str1,string str2,string result){
	quads.push_back(Quad{op,str1,str2,result});
}

//����һ����ʱ������
string newtemp(){
	string x="t"+to_string(tempVarNum);
	tempVarNum++;
	return x;
}

//�������������
void SemanticAnalyzer(){
	int i=0;
	for(auto q:quads){
		cout<<i<<" : "<<q.op<<" "<<q.str1<<" "<<q.str2<<" "<<q.result<<endl;
		i++;
	}
}

//-----------------------------------------------------------------------------------------
//�﷨������
AttrGroup expr();
AttrGroup term();
AttrGroup rest5(string inArray);
AttrGroup rest6(string inArray);
AttrGroup factor();
AttrGroup unary();

void equality();
void rest4();
void rel();
void rop_expr();
void stmts();
void rest0();
void stmt();
AttrGroup loc();
void boolExpr();
AttrGroup resta(string inArray);
void elist();
void rest1();


void error(){
	cout<<"error"<<endl;
	exit(0);
}

void success(){
	cout<<"success"<<endl;
}


/*����ʽ��
stmts -> stmt rest0
rest0 -> stmt rest0|$
stmt -> loc = expr ;
loc -> id resta 
resta -> [elist] | $

elist -> expr rest1
resft1 -> , expr rest1|$
bool -> equality
equality -> rel rest4
rest4 ->  == rel rest4|!= rel rest4|$
rel -> expr rop_expr
rop_expr -> < expr|<=expr|>expr|>=expr

expr -> term rest5 ;
rest5 -> + term rest5|- term rest5|$
term -> unary rest6
rest6 -> * unary rest6|/ unary rest6|$
unary -> factor
factor-> (expr)|loc|num
*/

AttrGroup expr(){
	AttrGroup attr;
	cout<<"expr -> term rest5"<<endl;
	string term_place = term().place;
	string rest5_place=rest5(term_place).place;
	attr.place=rest5_place;
	return attr;
	
}

AttrGroup term(){
	AttrGroup attr;
	cout<<"term->unary rest6"<<endl;
	string unary_place= unary().place;
	string rest6_place=rest6(unary_place).place;
	attr.place=rest6_place;
	return attr;
}

AttrGroup rest5(string inArray){
	AttrGroup attr;
	Word w=getNextWord();
	if(w.code==TK_PLUS){//+
		cout<<"rest5 -> + term rest5"<<endl;
		string rest5_1_inArray=newtemp();
		string term_place=term().place;
		emit("+",inArray,term_place,rest5_1_inArray);
		string rest5_1_place=rest5(rest5_1_inArray).place;
		attr.place=rest5_1_place;
		return attr;

	}else if(w.code==TK_MINUS){//-
		cout<<"rest5 -> - term rest5"<<endl;
		string rest5_1_inArray=newtemp();
		string term_place=term().place;
		emit("-",inArray,term_place,rest5_1_inArray);
		string rest5_1_place=rest5(rest5_1_inArray).place;
		attr.place=rest5_1_place;
		return attr;
	}else {
		rollBack(w);
		cout<<"rest5 -> $"<<endl;	
		string rest5_place=inArray;
		attr.place=rest5_place;
		return attr;
	}
}

AttrGroup rest6(string inArray){
	Word w=getNextWord();
	AttrGroup attr;
	if(w.code==TK_STAR){//*
		cout<<"rest6 -> * unary rest6"<<endl;
		string rest6_1_inArray=newtemp();
		string unary_place=unary().place;
		emit("*",inArray,unary_place,rest6_1_inArray);
		string rest6_1_place=rest6(rest6_1_inArray).place;
		attr.place=rest6_1_place;
		return attr;
	}else if(w.code==TK_DIVIDE){///
		cout<<"rest6 -> / unary rest6"<<endl;
		string rest6_1_inArray=newtemp();
		string unary_place=unary().place;
		emit("/",inArray,unary_place,rest6_1_inArray);
		string rest6_1_place=rest6(rest6_1_inArray).place;
		attr.place=rest6_1_place;
		return attr;
	}else {
		rollBack(w);
		cout<<"rest6 -> $"<<endl;
		string rest6_place=inArray;
		attr.place=rest6_place;
		return attr;
	}
}

AttrGroup unary(){
	AttrGroup attr;
	cout<<"unary->factor"<<endl;
	string unary_place=factor().place;
	attr.place=unary_place;
	return attr;
}

AttrGroup factor(){
	Word w=getNextWord();
	AttrGroup attr;
	if(w.code==TK_OPENPA){
		cout<<"factor -> (expr)"<<endl;
		string factor_place=expr().place;
		w=getNextWord();
		if(w.code!=TK_CLOSEPA){
			error();
			rollBack(w);
		}
		attr.place=factor_place;
		return attr;
	}else if(w.code==30){
		cout<<"factor->num"<<endl;
		string factor_place=w.value;
		attr.place=factor_place;
		return attr;
	}
	else{
		rollBack(w);
		cout<<"factor -> loc"<<endl;
		string loc_place=loc().place;
		string factor_place=loc_place;
		attr.place=factor_place;
		return attr;
	}

}

//ƥ�䲼�����ʽ

void boolExpr(){
	cout<<"bool->equality"<<endl;
	equality();
}

void equality(){
	cout<<"equality->rel rest4"<<endl;
	rel();
	rest4();
}

void rest4(){
	Word w=getNextWord();
	if(w.code==TK_EQ){
		cout<<"rest4-> == rel rest4"<<endl;
		rel();
		rest4();
	}
	else if(w.code==TK_NOTEQ){
		cout<<"rest4 -> != rel rest4"<<endl;
		rel();
		rest4();
	}else
	{
		rollBack(w);
		cout<<"rest4 -> $"<<endl;
	}
}

void rel(){
	cout<<"rel->expr rop_expr"<<endl;
	expr();
	rop_expr();
}

void rop_expr(){
	Word w=getNextWord();
	int code=w.code;
	if(code==TK_LT){
		cout<<"rop_expr -> < expr"<<endl;
		expr();
	}else if(code==TK_LEQ){
		cout<<"rop_expr -> <= expr"<<endl;
		expr();
	}else if(code==TK_GT){
		cout<<"rop_expr -> > expr"<<endl;
		expr();
	}else if(code==TK_GEQ){
		cout<<"rop_expr -> >= expr"<<endl;
		expr();
	}else{
		rollBack(w);
		cout<<"rop_expr -> $"<<endl;
	}
}

void stmts(){
	cout<<"stmts -> stmt rest0"<<endl;
	stmt();
	rest0();
}

void stmt(){
	Word w=getNextWord();

	if (w.code == KW_IF) {
			cout<<"stmt -> if(bool) stmt else stmt"<<endl;
			w=getNextWord();
			if(w.code==TK_OPENPA)
				boolExpr();
			else
				error();
			w=getNextWord();
			if(w.code==TK_CLOSEPA)
				 stmt();
			else 
				error();
            w = getNextWord();
            if (w.code == KW_ELSE) {
                stmt();
            }
        } else if (w.code == KW_WHILE) {
			cout<<"stmt -> while(bool) stmt"<<endl;
			w=getNextWord();
			if(w.code==TK_OPENPA){
				boolExpr();
			}else
				error();
			w=getNextWord();
			if(w.code==TK_CLOSEPA){
				stmt();
			}else
				error();
    }else{
		rollBack(w);
		cout<<"stmt -> loc = expr ;"<<endl;
		string loc_place=loc().place;
		w=getNextWord();
		if(w.code!=TK_ASSIGN)
			error();
		string expr_place=expr().place;
		w=getNextWord();
		if(w.code!=TK_SEMOCOLOM)
			error();
		emit("=",expr_place," ",loc_place);
	}
    
}

AttrGroup loc(){
	Word w=getNextWord();
	AttrGroup attr;
	if(w.code==TK_IDENT){
		cout<<"loc -> id resta"<<endl;
		string resta_plcae=resta(w.value).place;
		attr.place=resta_plcae;
	}else{
		error();
	}
	return attr;
}

void rest0(){
	    Word w=getNextWord();
		rollBack(w);//�ȶ�ȡ��һ�ַ��ڻ��ˣ���̽����һ�ַ��Ƿ�ΪEOF����ΪEOF���ټ����ݹ��﷨��
		if(!isEOF){
			cout<<"rest0 -> stmt rest0"<<endl;
			stmt();
			rest0();
		}

}

AttrGroup resta(string inArray){
	AttrGroup attr;
	Word w=getNextWord();
	if(w.code==TK_OPENBR){
		cout<<"resta -> [elist]"<<endl;
		elist();
		w=getNextWord();
		if(w.code!=TK_CLOSEBR)
			error();
	}
	else{
		rollBack(w);
		attr.place=inArray;
	 	cout<<"resta -> $"<<endl;
	}
	return attr;
}

void elist(){
	cout<<"elist -> expr rest1"<<endl;
	expr();
	rest1();
}

void rest1(){
	Word w=getNextWord();
	if(w.code==TK_COMMA){
		cout<<"rest1 -> , expr rest1"<<endl;
		expr();
		rest1();
	}else{
		rollBack(w);
		cout<<"rest1 -> $"<<endl;
	}
}


//----------------------------------------------------------
//�﷨���������
void parsing(){
	stmts();//���﷨����
	success();
	SemanticAnalyzer();//���������
}

//--------------------------------------------------------------------------------------------


int main(){

	string filename;		//�ļ�·��
	cout << "input your filePath��" << endl;
	while (true) {
		cin >> filename;		//��ȡ�ļ�·��
		if ((fopen_s(&fp,filename.c_str(), "r"))==0)		//���ļ�
			break;
		else
			cout << "FilePath Error��" << endl;	//��ȡʧ��
	}

	parsing();

	fclose(fp);
}


