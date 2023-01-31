#include  <stdio.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <string.h>
#include  "conio.h"

ushort variable['Z'-'A'+1]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char *s;
u_char memory[65536];
char strbuff[256];
char pbuff[0x8000];
int psize=0;
int ln=0;
void *stack[165536];
int  sp=0;
int tron=0;
ushort mod=0;

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
    printf("\nSyntaxerror in %d\n",ln);
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
    if (*s=='(') {
        skipc('(');
        v=expression();
        skipc(')'); 
        return(v);
        }
    else if (c=var()) {
        if (*s==':') {
            skipc(':');
            v=expression();
            skipc(')');
            v=memory[variable[toupper(c)-'A']+v];
            return(v);
            }
        else if (*s=='(') {
            skipc('(');
            v=expression();
            skipc(')');
            v=(((u_char)memory[variable[toupper(c)-'A']+v*2+1])<<8)+((u_char)memory[variable[toupper(c)-'A']+v*2]);
            return(v);
            }
        else {
            v=variable[toupper(c)-'A'];
            return(v);
            }
        }
    else if (*s=='$' && !isxdigit(*(s+1))) {
        s++;
        return(getch());
        }
    else if (*s=='?') {
        int i;
        char buf[256];
        s++;
        scanf("%s",buf);
        if (buf[0]=='$') {
            sscanf(buf+1,"%x",&i);
            }
        else {
            sscanf(buf,"%d",&i);
            }
        return((ushort)i);
        }
    else if (v=const_())
        return(v);
    else if (c=operator1(*s)) {
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
   v=term();
   while(c=operator2()) {
     v2=term();
     switch (c) {
     case '+':
       v+=v2;
       break;
     case '-':
       v-=v2;
       break;
     case '*':
       v*=v2;
       break;
     case '/':
       v/=v2;
       mod=v%v2;
       break;
     case '=':
       v=(v==v2);
       break;
     case '<':
       v=(v<v2);
       break;
     case 'N':
       v=(v!=v2);
       break;
     case 'A':
       v=(v<=v2);
       break;
     case '>':
       v=(v>v2);
       break;
     case 'B':
       v=(v>=v2);
       break;
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
   if (s==pbuff+psize) { 
       return(-1); }
   char *p=s;
   ushort n=getval10();
   s=p;
   if (n==-1) {
      return(-1);
      }
   if (n>=v)
     return(n);
   else
     newline();
   }
}
 
int go_to(ushort v) {
 if (v==-1)
     ln=-1;
 else {
     ln=searchline(v);
     }
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
    skipc('(');
    v= expression();
    skipc(')');
    if (!v) {
        s=p;
        sp+=1;
        }
}

int next_() {
    ushort *addr,to_,v;
    char *p;
    to_=(ushort )(long)stack[--sp];
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

int skipspc() {
  while(*s==' ')
   s++;
}

int randseed(ushort v) {
    srand(v);
}
int optional_command() {
     char c1,c2;
     c1=toupper(*s++);
     c2=toupper(*s++);
     if (c1=='L' && c2=='D') {
         load_src();
         }
    else if (c1=='Q'&&c2=='U') {
        exit(0);
        }
    else if (c1=='T'&&c2=='N') {
        tron=1;
        }
    else if (c1=='T'&&c2=='F') {
        tron=0;
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
            v=term();
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
        ushort *addr;
        if (*s==':') {
            ushort v2;
            s++;
            v=expression();
            skipc(')');
            skipc('=');
            v2=term();
            memory[variable[toupper(c)-'A']+v]=v2&0xff;
            continue;
            }
        else if (*s=='(') {
            ushort v2;
            s++;
            v=expression();
            skipc(')');
            skipc('=');
            v2=term();
            addr=(ushort *)&(memory[variable[toupper(c)-'A']+v*2]);
            *addr=v2;
            }
        else {
            ushort v;
            skipc('=');
            v=expression();
            addr=(ushort *)(&(variable[toupper(c)-'A']));
            *addr=v;
            }
        if (*s==',') {
            ushort to_;
            s++;
            v=expression();
            to_=v;
            stack[sp++]=(char *)addr;
            stack[sp++]=s;
            stack[sp++]=(char *)(long)to_;
            }
        continue;
        }
    syntaxerror();
    return(0);
    }
  }
}
               
int commandline() {
   char lb[256];
   char *f;
   ushort v;
   while(1) {
     printf("\n*Ready.\n");
     f=fgets(lb,sizeof(lb)-3,stdin);
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

