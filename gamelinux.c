
#include  <stdio.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <string.h>
#include <termios.h>
#include <unistd.h>

ushort variable['Z'-'A'+1]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char *s;
u_char memory[65536];
char strbuff[256];
char pbuff[65536];
int psize=0;
int ln=0;
void *stack[65536];
int  sp=0;
int tron=0;
ushort mod=0;
int for_mode=0;

int getch(){
      struct termios oldt, newt;
      int c;
      tcgetattr( STDIN_FILENO, &oldt );
      newt = oldt;
      newt.c_lflag &= ~ICANON;
      newt.c_lflag &= ~ECHO;
      tcsetattr( STDIN_FILENO, TCSANOW, &newt );
      c = getchar();
      tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
      return c;
}

int skipspc() {
  while(*s==' ')
   s++;
}


int load_source(char *s) {
  FILE *ifp=fopen(s,"rb");
  int idx=0;
  if (ifp) {
    while(idx<sizeof(pbuff)-1) {
       int c;
       c=fgetc(ifp);
       if (c==EOF) break;
       pbuff[idx]=c;
       idx++;
       }
    fclose(ifp);
    }
  psize=idx;
  pbuff[idx]='\0';
}

int load_src() {
    char fn[256],*op=fn;
    while(*s==' ') s++;
    while(*s!='\n') *op++=*s++;
    *op='\0';
    load_source(fn);
}


int syntaxerror() {
    printf("\nSyntaxerror in %d",ln);
    fflush(stdout);
}

int skipc(char c) {
    if (*s==c) { s++; return(0); }
    syntaxerror();
}

char operator2() {
     char c;
     switch (c=*s) {
     case '=': case '+': case '-': case'*': case '/':
         s++;
         return(c);
     case '<':
         s++;
         if (*s=='>') {
             s++;
             return('N');
             }
         else if (*s=='=') {
             s++;
             return('A');
             }
         else return('<');
     case '>':
         s++;
         if (*s=='=') {
             s++;
             return('B');
             }
         else return('>');
     default:
         return(0);
     }
}

char operator1() {
     char c;
     switch (c=*s) {
     case '+': case '-': case '\'': case '#': case '%':
         s++;
         return(c);
     default:
         return(0);
     }
}


char var() {
    char c=0;
    if (isalpha(*s)) {
        c=*s;
        while(isalpha(*s))
            ++s;
       }
    return(c);
}

ushort getval16() {
     int i;
     if (!isxdigit(*s)) return(-1);
     sscanf(s,"%x",&i);
     while(isxdigit(*s)) ++s;
     return((ushort)i);
}
     
ushort getval10() {
     ushort v;
     if (!isdigit(*s)) return(-1);
     v=atoi(s);
     while(isdigit(*s))
        ++s;
     return(v);
}

char *str() {
    char *si=strbuff;
    if (*s=='"') {
        skipc('"');
        while(*s!='"')
           *si++=*s++;
        skipc('"');
        }
    *si='\0';
    return(strbuff);
}

ushort const_() {
    ushort v;
    if (*s=='"') {
        char *si;
        si=str();
        v=si[0]+si[1]*256;
        return(v);
        }
    else if (*s=='$') {
        skipc('$');
        v=getval16();
        return(v);
        }
    else if (isdigit(*s)) {
        v=getval10();
        return(v);
        }
   return(0);
}
    
ushort expression();

ushort term() {
    ushort v;
    char c;
    skipspc();
    if (*s=='(') {            // (exp)
        skipc('(');
        v=expression();
        skipspc();
        skipc(')'); 
        return(v);
        }
    else if (c=var()) {
        if (*s==':') {       // V:exp)
            skipc(':');
            v=expression();
            skipc(')');
            v=memory[variable[toupper(c)-'A']+v];
            return(v);
            }
        else if (*s=='(') {  // V(exp)
            skipc('(');
            v=expression();
            skipc(')');
            v=(((u_char)memory[variable[toupper(c)-'A']+v*2+1])<<8)+((u_char)memory[variable[toupper(c)-'A']+v*2]);
            return(v);
            }
        else {               // V
            v=variable[toupper(c)-'A'];
            return(v);
            }
        }
    else if ((*s=='$') && (!isxdigit(*(s+1)))) { // getch
        s++;
        return(getch());
        }
    else if (*s=='?') { // input
        int i;
        char buf[256];
        s++;
        scanf("%s",buf);
        if (buf[0]=='$')
            sscanf(buf+1,"%x",&i);
        else
            sscanf(buf,"%d",&i);
        return((ushort)i);
        }
    else if (v=const_())  // const
        return(v);
    else if (c=operator1(*s)) {  // term
        v=term();
        if (c=='-')
           return(-v);
       else if (c=='+')
           return(v<0?-v:v);
       else if (c=='#')
           return(!(v));
       else if (c=='\'') {
           int i;
           i=(ushort)(rand()%v);
           return(i);
           }
       else if (c=='%')
           return(mod);
    }
}

ushort expression() {
   ushort v,v2;
   char c;
   skipspc();
   v=term();
   while(1) {
     if ((c=operator2())==0) break;
     v2=term();
     switch (c) {
     case '+': v+=v2; break;
     case '-': v-=v2; break;
     case '*': v*=v2; break;
     case '/': mod=v%v2; if (v2==0) {printf("Division by zero\n");v=-1;break; }
                         else { v/=v2; break; }
     case '=': v=(v==v2); break;
     case '<': v=(v<v2); break;
     case 'N': v=(v!=v2); break;
     case 'A': v=(v<=v2); break;
     case '>': v=(v>v2); break;
     case 'B': v=(v>=v2); break;
       }
     }
  return(v);
}

int newline() {
    while(1) {
       if (*s=='\n' || *s=='\0') break;
       s++;
       }
    if (*s=='\n') s++;
}

int searchline(ushort v) {
 s=pbuff;
 if (s[0]=='#')
    while(*s++!='\n' );
 while(1) {
   if (s==pbuff+psize)
       return(-1);
   char *p=s;
   ushort n=getval10();
   s=p;
   if (n==-1)
      return(-1);
   if (n>=v)
     return(n);
   else
     newline();
   }
}
 
int go_to(ushort v) {
 if (v==-1)
     ln=-1;
 else
     ln=searchline(v);
}

int go_sub(ushort v) {
 stack[sp++]=s;
 go_to(v);
}

int return_() {
 s=stack[--sp];
}

int do_() {
    stack[sp++]=s;
}

int until_() {
    char *p;
    ushort v;
    p=stack[--sp];
    v= expression();
    if (!v) {
        s=p;
        sp+=1;
        }
}

int next_() {
    ushort *addr,to_,v;
    char *p;
    to_=(ushort )(ulong)stack[--sp];
    p=stack[--sp];
    addr=stack[--sp];
    *addr=v=expression();
    if (v<=to_) {
        s=p;
        sp+=3;
        }
}

int if_(ushort v) {
  if (v==0) { newline(); --s;
      }
}

int randseed(ushort v) {
    srand(v);
}

int optional_command() {
     char c1,c2;
     c1=toupper(*s++);
     c2=toupper(*s++);
     if (c1=='L' && c2=='D')
         load_src();
    else if (c1=='Q'&&c2=='U')
        exit(0);
    else if (c1=='T'&&c2=='N')
        tron=1;
    else if (c1=='T'&&c2=='F')
        tron=0;
    else if (c1=='S'&&c2=='H')
        system("bash");
    else if (c1=='F'&&c2=='M') {
       ushort v=expression();
       for_mode=v;
       }
    else syntaxerror();
}

int gameint() {
    char c;
    ushort v;
    while(1) { /* line */
    c=*s;
    if (c=='\0')
        return(0);
    if (ln==-1)
        return(0);
    if (ln) {
        v=getval10();
        if (tron) { printf("[%d]",v); fflush(stdout); }
        ln=v;
        if (*s!=' ') {
            newline();
            continue;
            } 
        }
    while(1) {
    c=*s;
    if (c=='\0') return(0);
    else if (c=='\n') { s++; break; }
    else if (c==' ') { skipspc(); continue;}
    else if (c=='"') { printf("%s",str()); fflush(stdout); continue; }
    else if (c=='/') { skipc('/'); printf("\n"); continue; }
    else if (c=='.') { skipc('.'); skipc('='); 
        v= expression();
        for(int i=0;i<v;i++)
            printf(" ");
        fflush(stdout);
        continue;
        }
    else if (c=='*') {
        s++;
        optional_command();
        continue;
        }
    else if (c=='?') { s++;
        c=*s;
        if (c=='=') {
            skipc('='); v=expression(); printf("%d",v); fflush(stdout);
            continue;
            }
        else if (c=='?') {
            skipc('?'); skipc('='); v=expression(); printf("%04hx",v&0xffff);
            fflush(stdout);
            continue;
            }
        else if (c=='$') {
            skipc('$'); skipc('='); v=expression(); printf("%02x",v&0xff);
            fflush(stdout);
            continue;
            }
        else if (c=='(') {
            char buf[12],b2[128];
            int v2;
            skipc('(');
            v2=expression();
            skipc(')');
            skipc('=');
            v=expression();
            snprintf(buf,12,"%d",v2);
            b2[0]='\0';
            strcat(b2,"%");
            strcat(b2,buf);
            strcat(b2,"d");
            printf(b2,v);
            fflush(stdout);
            continue;
            }
       syntaxerror();
       return(0);
       }
    else if (c=='\'') { s++; skipc('='); v=expression(); randseed(v); continue; }
    else if (c=='$') { s++; skipc('='); v=expression(); printf("%c",v); 
            fflush(stdout);
            continue;
          }
    else if (c=='#') { s++; skipc('='); v=expression(); go_to(v); break; }
    else if (c=='!') { s++; skipc('='); v=expression(); go_sub(v); break; }
    else if (c==']') { s++; return_(); continue; }
    else if (c=='@') { s++;
        if (*s=='=') {
             skipc('=');
             if (*s=='(') { /* until */
                 until_(v);
                 continue;
                 }
             else { /* next */
                 next_();
                 continue;
                 }
             }
        else { /* do */
             do_();
             continue;
             } 
        }
    else if (c==';') { s++;
         skipc('='); v=expression(); if_(v); 
         continue;
         }
    else if (c=var()) {
        ushort v,*addr;
        if (*s==':') {
            ushort v2,vidx;
            s++;
            vidx=expression();
            skipc(')');
            skipc('=');
            v2=expression();
            memory[variable[toupper(c)-'A']+vidx]=v2&0xff;
            continue;
            }
        else if (*s=='(') {
            ushort vidx;
            s++;
            vidx=expression();
            skipc(')');
            skipc('=');
            v=expression();
            addr=(ushort *)&(memory[variable[toupper(c)-'A']+vidx*2]);
            *addr=v;
            }
        else {
            skipc('=');
            v=expression();
            addr=(ushort *)(&(variable[toupper(c)-'A']));
            *addr=v;
            }
        if (*s==',') {
            ushort to_;
            skipc(',');
            to_=expression();
            if (v>to_ && for_mode==0) { // if initial_value > to_value, skip to the end of next NEXT
              int eof=0;
              while(*s!='@') {
                  if (*s=='\0') { eof=1; break; }
                  s++;
                  }
              if (!eof) {
                skipc('@');
                skipc('=');
                expression();
                }
              }
            else {
              stack[sp++]=(char *)addr;
              stack[sp++]=s;
              stack[sp++]=(char *)(ulong)to_;
              }
            }
        continue;
        }
    syntaxerror();
    return(0);
    }
  }
}

int title() {
    printf("gamelinux ver 0.9.2 Copyright 2023 (C) by Taisuke Maekawa\n");
}
               
int commandline() {
   char lb[256];
   char *f;
   ushort v;
   title();
   while(1) {
     printf("\n*Ready.\n");
     f=fgets(lb,sizeof(lb)-1,stdin);
     if (f==NULL)
        exit(1);
     ln=0;
     sp=0;
     s=lb;
     gameint();
     }
}
 
int main(int argc,char *argv[]) {
    psize=0;
    if (argc==2) {
       load_source(argv[1]);
       go_to(1);
       gameint();
       }
    else commandline();
}
