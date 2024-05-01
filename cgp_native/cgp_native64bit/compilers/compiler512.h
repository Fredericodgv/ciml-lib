// typedef __m512i nodetype;
// nodetype* nodeoutput;

void cgp_init_consts() {}

#define MAXCODESIZE 289
typedef void evalfunc(void);
unsigned char* code[MAX_POPSIZE];
void cgp_compile(unsigned char* pcode, chromosome p_chrom, int* isused)
{
    #define C(val)  *(pcode++)=(val)
    #define CI(val) {*((uint32_t *)(pcode))= (uint32_t)((__PTRDIFF_TYPE__)val); pcode += sizeof(uint32_t);}
    #define CL(val) {*((uint64_t *)(pcode))= (uint64_t)((__PTRDIFF_TYPE__)val); pcode += sizeof(uint64_t);}

    int pnodeout = 0;
    int in1,in2,fce;
    int out = params.inputs - 1;

    /// Save modified registers
    /// %rsp, %xmm0, %rbp, %rbx, %r10, %xmm1, %ymm1, %ymm0, %rax, %xmm2
    C(0x41);C(0x52);                                  //push   %r10
    C(0x53);                                          //push   %rbx
    #warning "These registers are modified but not restored: %rbp %xmm0 %xmm1 %xmm2 %ymm0 %ymm1"
    /// Load nodeoutputs -> rbx
    C(0x48);C(0xbb);CL(nodeoutput);
    /// Native code generation
    for (int i=0; i < params.cols; i++)
        for (int j=0; j < params.rows; j++) 
        { 
            in1 = *p_chrom++; in2 = *p_chrom++; fce = *p_chrom++; out++;
            #ifdef DONOTEVALUATEUNUSEDNODES
            if (!isused[out]) continue;
            #endif
            switch (fce)
            {
              case 0:
                  ///nodeoutput[out] = nodeoutput[in1];
                  C(0x4c);C(0x8d);C(0x54);C(0x24);C(0x08);          //0/0 lea    0x8(%rsp),%r10
                  C(0x48);C(0x83);C(0xe4);C(0xc0);                  //5/5 and    $0xffffffffffffffc0,%rsp
                  C(0x41);C(0xff);C(0x72);C(0xf8);                  //9/9 pushq  -0x8(%r10)
                  C(0x55);                                          //d/d push   %rbp
                  C(0x48);C(0x89);C(0xe5);                          //e/e mov    %rsp,%rbp
                  C(0x41);C(0x52);                                  //11/11 push   %r10
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1);       //13/13 vmovdqa in1(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out);       //1b/1b vmovaps %xmm0,out(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+16);    //23/23 vmovdqa in1+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+16);    //2b/2b vmovaps %xmm0,out+16(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+32);    //33/33 vmovdqa in1+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+32);    //3b/3b vmovaps %xmm0,out+32(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+48);    //43/43 vmovdqa in1+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+48);    //4b/4b vmovaps %xmm0,out+48(%rbx)
                  C(0x90);                                          //53/53 nop
                  C(0x41);C(0x5a);                                  //54/54 pop    %r10
                  C(0x5d);                                          //56/56 pop    %rbp
                  C(0x49);C(0x8d);C(0x62);C(0xf8);                  //57/57 lea    -0x8(%r10),%rsp
                  //code size: 91, stack requirements: 0
                  //registers: %rsp, %xmm0, %rbp, %rbx, %r10
                  break;
              case 1:
                  ///nodeoutput[out] = ~nodeoutput[in1];
                  C(0x4c);C(0x8d);C(0x54);C(0x24);C(0x08);          //0/0 lea    0x8(%rsp),%r10
                  C(0x48);C(0x83);C(0xe4);C(0xc0);                  //5/5 and    $0xffffffffffffffc0,%rsp
                  C(0x48);C(0x89);C(0xd8);                          //9/9 mov    %rbx,%rax
                  C(0x41);C(0xff);C(0x72);C(0xf8);                  //c/c pushq  -0x8(%r10)
                  C(0x55);                                          //10/10 push   %rbp
                  C(0x48);C(0x89);C(0xe5);                          //11/11 mov    %rsp,%rbp
                  C(0x41);C(0x52);                                  //14/14 push   %r10
                  C(0x48);C(0x83);C(0xec);C(0x70);                  //16/16 sub    $0x70,%rsp
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1);       //1a/1a vmovdqa in1(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//22/22 vmovaps %xmm0,-0xb0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+16);    //2a/2a vmovdqa in1+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x60);C(0xff);C(0xff);C(0xff);//32/32 vmovaps %xmm0,-0xa0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+32);    //3a/3a vmovdqa in1+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//42/42 vmovaps %xmm0,-0x90(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+48);    //4a/4a vmovdqa in1+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x45);C(0x80);          //52/52 vmovaps %xmm0,-0x80(%rbp)
                  C(0xc5);C(0xfd);C(0x76);C(0xc0);                  //57/57 vpcmpeqd %ymm0,%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0x8d);C(0x50);C(0xff);C(0xff);C(0xff);//5b/5b vpxor  -0xb0(%rbp),%ymm0,%ymm1
                  C(0xc5);C(0xfd);C(0xef);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//63/63 vpxor  -0x90(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0x7f);C(0x8d);C(0x10);C(0xff);C(0xff);C(0xff);//6b/6b vmovdqa %ymm1,-0xf0(%rbp)
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//73/73 vmovdqa %ymm0,-0xd0(%rbp)
                  C(0xc5);C(0xf8);C(0x29);C(0x88);CI(64*out);       //7b/7b vmovaps %xmm1,0x7f880(%rax) # !PLEASE CHECK THIS
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0x20);C(0xff);C(0xff);C(0xff);//83/83 vmovdqa -0xe0(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+16);    //8b/8b vmovaps %xmm0,out+16(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//93/93 vmovdqa -0xd0(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+32);    //9b/9b vmovaps %xmm0,out+32(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0x40);C(0xff);C(0xff);C(0xff);//a3/a3 vmovdqa -0xc0(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+48);    //ab/ab vmovaps %xmm0,out+48(%rbx)
                  C(0x90);                                          //b3/b3 nop
                  C(0xc5);C(0xf8);C(0x77);                          //b4/b4 vzeroupper
                  C(0x48);C(0x83);C(0xc4);C(0x70);                  //b7/b7 add    $0x70,%rsp
                  C(0x41);C(0x5a);                                  //bb/bb pop    %r10
                  C(0x5d);                                          //bd/bd pop    %rbp
                  C(0x49);C(0x8d);C(0x62);C(0xf8);                  //be/be lea    -0x8(%r10),%rsp
                  //code size: 194, stack requirements: 0
                  //registers: %rbx, %r10, %xmm0, %rbp, %xmm1, %ymm1, %ymm0, %rsp, %rax
                  break;
              case 2:
                  ///nodeoutput[out] = nodeoutput[in1] & nodeoutput[in2];
                  C(0x4c);C(0x8d);C(0x54);C(0x24);C(0x08);          //0/0 lea    0x8(%rsp),%r10
                  C(0x48);C(0x83);C(0xe4);C(0xc0);                  //5/5 and    $0xffffffffffffffc0,%rsp
                  C(0x41);C(0xff);C(0x72);C(0xf8);                  //9/9 pushq  -0x8(%r10)
                  C(0x55);                                          //d/d push   %rbp
                  C(0x48);C(0x89);C(0xe5);                          //e/e mov    %rsp,%rbp
                  C(0x41);C(0x52);                                  //11/11 push   %r10
                  C(0x48);C(0x81);C(0xec);C(0xb0);C(0x00);C(0x00);C(0x00);//13/13 sub    $0xb0,%rsp
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1);       //1a/1a vmovdqa in1(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//22/22 vmovaps %xmm0,-0xf0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+16);    //2a/2a vmovdqa in1+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x20);C(0xff);C(0xff);C(0xff);//32/32 vmovaps %xmm0,-0xe0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+32);    //3a/3a vmovdqa in1+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//42/42 vmovaps %xmm0,-0xd0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+48);    //4a/4a vmovdqa in1+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x40);C(0xff);C(0xff);C(0xff);//52/52 vmovaps %xmm0,-0xc0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2);       //5a/5a vmovdqa in2(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//62/62 vmovaps %xmm0,-0xb0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+16);    //6a/6a vmovdqa in2+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x60);C(0xff);C(0xff);C(0xff);//72/72 vmovaps %xmm0,-0xa0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+32);    //7a/7a vmovdqa in2+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//82/82 vmovaps %xmm0,-0x90(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+48);    //8a/8a vmovdqa in2+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x45);C(0x80);          //92/92 vmovaps %xmm0,-0x80(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//97/97 vmovdqa -0xf0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xdb);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//9f/9f vpand  -0xb0(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xf9);C(0x6f);C(0xc8);                  //a7/a7 vmovdqa %xmm0,%xmm1
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xd0);C(0xfe);C(0xff);C(0xff);//ab/ab vmovdqa %ymm0,-0x130(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//b3/b3 vmovdqa -0xd0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xdb);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//bb/bb vpand  -0x90(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//c3/c3 vmovdqa %ymm0,-0x110(%rbp)
                  C(0xc5);C(0xf8);C(0x29);C(0x8b);CI(64*out);       //cb/cb vmovaps %xmm1,out(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xe0);C(0xfe);C(0xff);C(0xff);//d3/d3 vmovdqa -0x120(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+16);    //db/db vmovaps %xmm0,out+16(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//e3/e3 vmovdqa -0x110(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+32);    //eb/eb vmovaps %xmm0,out+32(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0x00);C(0xff);C(0xff);C(0xff);//f3/f3 vmovdqa -0x100(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+48);    //fb/fb vmovaps %xmm0,out+48(%rbx)
                  C(0x90);                                          //103/103 nop
                  C(0xc5);C(0xf8);C(0x77);                          //104/104 vzeroupper
                  C(0x48);C(0x81);C(0xc4);C(0xb0);C(0x00);C(0x00);C(0x00);//107/107 add    $0xb0,%rsp
                  C(0x41);C(0x5a);                                  //10e/10e pop    %r10
                  C(0x5d);                                          //110/110 pop    %rbp
                  C(0x49);C(0x8d);C(0x62);C(0xf8);                  //111/111 lea    -0x8(%r10),%rsp
                  //code size: 277, stack requirements: 0
                  //registers: %rbx, %r10, %xmm0, %rbp, %xmm1, %ymm0, %rsp
                  break;
              case 3:
                  ///nodeoutput[out] = nodeoutput[in1] | nodeoutput[in2];
                  C(0x4c);C(0x8d);C(0x54);C(0x24);C(0x08);          //0/0 lea    0x8(%rsp),%r10
                  C(0x48);C(0x83);C(0xe4);C(0xc0);                  //5/5 and    $0xffffffffffffffc0,%rsp
                  C(0x41);C(0xff);C(0x72);C(0xf8);                  //9/9 pushq  -0x8(%r10)
                  C(0x55);                                          //d/d push   %rbp
                  C(0x48);C(0x89);C(0xe5);                          //e/e mov    %rsp,%rbp
                  C(0x41);C(0x52);                                  //11/11 push   %r10
                  C(0x48);C(0x81);C(0xec);C(0xb0);C(0x00);C(0x00);C(0x00);//13/13 sub    $0xb0,%rsp
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1);       //1a/1a vmovdqa in1(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//22/22 vmovaps %xmm0,-0xf0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+16);    //2a/2a vmovdqa in1+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x20);C(0xff);C(0xff);C(0xff);//32/32 vmovaps %xmm0,-0xe0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+32);    //3a/3a vmovdqa in1+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//42/42 vmovaps %xmm0,-0xd0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+48);    //4a/4a vmovdqa in1+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x40);C(0xff);C(0xff);C(0xff);//52/52 vmovaps %xmm0,-0xc0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2);       //5a/5a vmovdqa in2(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//62/62 vmovaps %xmm0,-0xb0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+16);    //6a/6a vmovdqa in2+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x60);C(0xff);C(0xff);C(0xff);//72/72 vmovaps %xmm0,-0xa0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+32);    //7a/7a vmovdqa in2+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//82/82 vmovaps %xmm0,-0x90(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+48);    //8a/8a vmovdqa in2+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x45);C(0x80);          //92/92 vmovaps %xmm0,-0x80(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//97/97 vmovdqa -0xf0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xeb);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//9f/9f vpor   -0xb0(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xf9);C(0x6f);C(0xc8);                  //a7/a7 vmovdqa %xmm0,%xmm1
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xd0);C(0xfe);C(0xff);C(0xff);//ab/ab vmovdqa %ymm0,-0x130(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//b3/b3 vmovdqa -0xd0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xeb);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//bb/bb vpor   -0x90(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//c3/c3 vmovdqa %ymm0,-0x110(%rbp)
                  C(0xc5);C(0xf8);C(0x29);C(0x8b);CI(64*out);       //cb/cb vmovaps %xmm1,out(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xe0);C(0xfe);C(0xff);C(0xff);//d3/d3 vmovdqa -0x120(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+16);    //db/db vmovaps %xmm0,out+16(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//e3/e3 vmovdqa -0x110(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+32);    //eb/eb vmovaps %xmm0,out+32(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0x00);C(0xff);C(0xff);C(0xff);//f3/f3 vmovdqa -0x100(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+48);    //fb/fb vmovaps %xmm0,out+48(%rbx)
                  C(0x90);                                          //103/103 nop
                  C(0xc5);C(0xf8);C(0x77);                          //104/104 vzeroupper
                  C(0x48);C(0x81);C(0xc4);C(0xb0);C(0x00);C(0x00);C(0x00);//107/107 add    $0xb0,%rsp
                  C(0x41);C(0x5a);                                  //10e/10e pop    %r10
                  C(0x5d);                                          //110/110 pop    %rbp
                  C(0x49);C(0x8d);C(0x62);C(0xf8);                  //111/111 lea    -0x8(%r10),%rsp
                  //code size: 277, stack requirements: 0
                  //registers: %rbx, %r10, %xmm0, %rbp, %xmm1, %ymm0, %rsp
                  break;
              case 4:
                  ///nodeoutput[out] = nodeoutput[in1] ^ nodeoutput[in2];
                  C(0x4c);C(0x8d);C(0x54);C(0x24);C(0x08);          //0/0 lea    0x8(%rsp),%r10
                  C(0x48);C(0x83);C(0xe4);C(0xc0);                  //5/5 and    $0xffffffffffffffc0,%rsp
                  C(0x41);C(0xff);C(0x72);C(0xf8);                  //9/9 pushq  -0x8(%r10)
                  C(0x55);                                          //d/d push   %rbp
                  C(0x48);C(0x89);C(0xe5);                          //e/e mov    %rsp,%rbp
                  C(0x41);C(0x52);                                  //11/11 push   %r10
                  C(0x48);C(0x81);C(0xec);C(0xb0);C(0x00);C(0x00);C(0x00);//13/13 sub    $0xb0,%rsp
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1);       //1a/1a vmovdqa in1(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//22/22 vmovaps %xmm0,-0xf0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+16);    //2a/2a vmovdqa in1+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x20);C(0xff);C(0xff);C(0xff);//32/32 vmovaps %xmm0,-0xe0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+32);    //3a/3a vmovdqa in1+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//42/42 vmovaps %xmm0,-0xd0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+48);    //4a/4a vmovdqa in1+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x40);C(0xff);C(0xff);C(0xff);//52/52 vmovaps %xmm0,-0xc0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2);       //5a/5a vmovdqa in2(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//62/62 vmovaps %xmm0,-0xb0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+16);    //6a/6a vmovdqa in2+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x60);C(0xff);C(0xff);C(0xff);//72/72 vmovaps %xmm0,-0xa0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+32);    //7a/7a vmovdqa in2+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//82/82 vmovaps %xmm0,-0x90(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+48);    //8a/8a vmovdqa in2+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x45);C(0x80);          //92/92 vmovaps %xmm0,-0x80(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//97/97 vmovdqa -0xf0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//9f/9f vpxor  -0xb0(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xf9);C(0x6f);C(0xc8);                  //a7/a7 vmovdqa %xmm0,%xmm1
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xd0);C(0xfe);C(0xff);C(0xff);//ab/ab vmovdqa %ymm0,-0x130(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//b3/b3 vmovdqa -0xd0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//bb/bb vpxor  -0x90(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//c3/c3 vmovdqa %ymm0,-0x110(%rbp)
                  C(0xc5);C(0xf8);C(0x29);C(0x8b);CI(64*out);       //cb/cb vmovaps %xmm1,out(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xe0);C(0xfe);C(0xff);C(0xff);//d3/d3 vmovdqa -0x120(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+16);    //db/db vmovaps %xmm0,out+16(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//e3/e3 vmovdqa -0x110(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+32);    //eb/eb vmovaps %xmm0,out+32(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0x00);C(0xff);C(0xff);C(0xff);//f3/f3 vmovdqa -0x100(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+48);    //fb/fb vmovaps %xmm0,out+48(%rbx)
                  C(0x90);                                          //103/103 nop
                  C(0xc5);C(0xf8);C(0x77);                          //104/104 vzeroupper
                  C(0x48);C(0x81);C(0xc4);C(0xb0);C(0x00);C(0x00);C(0x00);//107/107 add    $0xb0,%rsp
                  C(0x41);C(0x5a);                                  //10e/10e pop    %r10
                  C(0x5d);                                          //110/110 pop    %rbp
                  C(0x49);C(0x8d);C(0x62);C(0xf8);                  //111/111 lea    -0x8(%r10),%rsp
                  //code size: 277, stack requirements: 0
                  //registers: %rbx, %r10, %xmm0, %rbp, %xmm1, %ymm0, %rsp
                  break;
              case 5:
                  ///nodeoutput[out] = ~(nodeoutput[in1] & nodeoutput[in2]);
                  C(0x4c);C(0x8d);C(0x54);C(0x24);C(0x08);          //0/0 lea    0x8(%rsp),%r10
                  C(0x48);C(0x83);C(0xe4);C(0xc0);                  //5/5 and    $0xffffffffffffffc0,%rsp
                  C(0xc5);C(0xf5);C(0x76);C(0xc9);                  //9/9 vpcmpeqd %ymm1,%ymm1,%ymm1
                  C(0x41);C(0xff);C(0x72);C(0xf8);                  //d/d pushq  -0x8(%r10)
                  C(0x55);                                          //11/11 push   %rbp
                  C(0x48);C(0x89);C(0xe5);                          //12/12 mov    %rsp,%rbp
                  C(0x41);C(0x52);                                  //15/15 push   %r10
                  C(0x48);C(0x81);C(0xec);C(0xb0);C(0x00);C(0x00);C(0x00);//17/17 sub    $0xb0,%rsp
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1);       //1e/1e vmovdqa in1(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//26/26 vmovaps %xmm0,-0xf0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+16);    //2e/2e vmovdqa in1+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x20);C(0xff);C(0xff);C(0xff);//36/36 vmovaps %xmm0,-0xe0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+32);    //3e/3e vmovdqa in1+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//46/46 vmovaps %xmm0,-0xd0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+48);    //4e/4e vmovdqa in1+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x40);C(0xff);C(0xff);C(0xff);//56/56 vmovaps %xmm0,-0xc0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2);       //5e/5e vmovdqa in2(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//66/66 vmovaps %xmm0,-0xb0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+16);    //6e/6e vmovdqa in2+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x60);C(0xff);C(0xff);C(0xff);//76/76 vmovaps %xmm0,-0xa0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+32);    //7e/7e vmovdqa in2+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//86/86 vmovaps %xmm0,-0x90(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+48);    //8e/8e vmovdqa in2+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x45);C(0x80);          //96/96 vmovaps %xmm0,-0x80(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//9b/9b vmovdqa -0xf0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xdb);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//a3/a3 vpand  -0xb0(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0xc1);                  //ab/ab vpxor  %ymm1,%ymm0,%ymm0
                  C(0xc5);C(0xf9);C(0x6f);C(0xd0);                  //af/af vmovdqa %xmm0,%xmm2
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xd0);C(0xfe);C(0xff);C(0xff);//b3/b3 vmovdqa %ymm0,-0x130(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//bb/bb vmovdqa -0xd0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xdb);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//c3/c3 vpand  -0x90(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0xc1);                  //cb/cb vpxor  %ymm1,%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//cf/cf vmovdqa %ymm0,-0x110(%rbp)
                  C(0xc5);C(0xf8);C(0x29);C(0x93);CI(64*out);       //d7/d7 vmovaps %xmm2,out(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xe0);C(0xfe);C(0xff);C(0xff);//df/df vmovdqa -0x120(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+16);    //e7/e7 vmovaps %xmm0,out+16(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//ef/ef vmovdqa -0x110(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+32);    //f7/f7 vmovaps %xmm0,out+32(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0x00);C(0xff);C(0xff);C(0xff);//ff/ff vmovdqa -0x100(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+48);    //107/107 vmovaps %xmm0,out+48(%rbx)
                  C(0x90);                                          //10f/10f nop
                  C(0xc5);C(0xf8);C(0x77);                          //110/110 vzeroupper
                  C(0x48);C(0x81);C(0xc4);C(0xb0);C(0x00);C(0x00);C(0x00);//113/113 add    $0xb0,%rsp
                  C(0x41);C(0x5a);                                  //11a/11a pop    %r10
                  C(0x5d);                                          //11c/11c pop    %rbp
                  C(0x49);C(0x8d);C(0x62);C(0xf8);                  //11d/11d lea    -0x8(%r10),%rsp
                  //code size: 289, stack requirements: 0
                  //registers: %rbx, %r10, %xmm2, %xmm0, %rbp, %ymm1, %ymm0, %rsp
                  break;
              case 6:
                  ///nodeoutput[out] = ~(nodeoutput[in1] | nodeoutput[in2]);
                  C(0x4c);C(0x8d);C(0x54);C(0x24);C(0x08);          //0/0 lea    0x8(%rsp),%r10
                  C(0x48);C(0x83);C(0xe4);C(0xc0);                  //5/5 and    $0xffffffffffffffc0,%rsp
                  C(0xc5);C(0xf5);C(0x76);C(0xc9);                  //9/9 vpcmpeqd %ymm1,%ymm1,%ymm1
                  C(0x41);C(0xff);C(0x72);C(0xf8);                  //d/d pushq  -0x8(%r10)
                  C(0x55);                                          //11/11 push   %rbp
                  C(0x48);C(0x89);C(0xe5);                          //12/12 mov    %rsp,%rbp
                  C(0x41);C(0x52);                                  //15/15 push   %r10
                  C(0x48);C(0x81);C(0xec);C(0xb0);C(0x00);C(0x00);C(0x00);//17/17 sub    $0xb0,%rsp
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1);       //1e/1e vmovdqa in1(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//26/26 vmovaps %xmm0,-0xf0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+16);    //2e/2e vmovdqa in1+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x20);C(0xff);C(0xff);C(0xff);//36/36 vmovaps %xmm0,-0xe0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+32);    //3e/3e vmovdqa in1+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//46/46 vmovaps %xmm0,-0xd0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+48);    //4e/4e vmovdqa in1+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x40);C(0xff);C(0xff);C(0xff);//56/56 vmovaps %xmm0,-0xc0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2);       //5e/5e vmovdqa in2(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//66/66 vmovaps %xmm0,-0xb0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+16);    //6e/6e vmovdqa in2+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x60);C(0xff);C(0xff);C(0xff);//76/76 vmovaps %xmm0,-0xa0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+32);    //7e/7e vmovdqa in2+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//86/86 vmovaps %xmm0,-0x90(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+48);    //8e/8e vmovdqa in2+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x45);C(0x80);          //96/96 vmovaps %xmm0,-0x80(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//9b/9b vmovdqa -0xf0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xeb);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//a3/a3 vpor   -0xb0(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0xc1);                  //ab/ab vpxor  %ymm1,%ymm0,%ymm0
                  C(0xc5);C(0xf9);C(0x6f);C(0xd0);                  //af/af vmovdqa %xmm0,%xmm2
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xd0);C(0xfe);C(0xff);C(0xff);//b3/b3 vmovdqa %ymm0,-0x130(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//bb/bb vmovdqa -0xd0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xeb);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//c3/c3 vpor   -0x90(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0xc1);                  //cb/cb vpxor  %ymm1,%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//cf/cf vmovdqa %ymm0,-0x110(%rbp)
                  C(0xc5);C(0xf8);C(0x29);C(0x93);CI(64*out);       //d7/d7 vmovaps %xmm2,out(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xe0);C(0xfe);C(0xff);C(0xff);//df/df vmovdqa -0x120(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+16);    //e7/e7 vmovaps %xmm0,out+16(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//ef/ef vmovdqa -0x110(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+32);    //f7/f7 vmovaps %xmm0,out+32(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0x00);C(0xff);C(0xff);C(0xff);//ff/ff vmovdqa -0x100(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+48);    //107/107 vmovaps %xmm0,out+48(%rbx)
                  C(0x90);                                          //10f/10f nop
                  C(0xc5);C(0xf8);C(0x77);                          //110/110 vzeroupper
                  C(0x48);C(0x81);C(0xc4);C(0xb0);C(0x00);C(0x00);C(0x00);//113/113 add    $0xb0,%rsp
                  C(0x41);C(0x5a);                                  //11a/11a pop    %r10
                  C(0x5d);                                          //11c/11c pop    %rbp
                  C(0x49);C(0x8d);C(0x62);C(0xf8);                  //11d/11d lea    -0x8(%r10),%rsp
                  //code size: 289, stack requirements: 0
                  //registers: %rbx, %r10, %xmm2, %xmm0, %rbp, %ymm1, %ymm0, %rsp
                  break;
              case 7:
                  ///nodeoutput[out] = ~(nodeoutput[in1] ^ nodeoutput[in2]);
                  C(0x4c);C(0x8d);C(0x54);C(0x24);C(0x08);          //0/0 lea    0x8(%rsp),%r10
                  C(0x48);C(0x83);C(0xe4);C(0xc0);                  //5/5 and    $0xffffffffffffffc0,%rsp
                  C(0xc5);C(0xf5);C(0x76);C(0xc9);                  //9/9 vpcmpeqd %ymm1,%ymm1,%ymm1
                  C(0x41);C(0xff);C(0x72);C(0xf8);                  //d/d pushq  -0x8(%r10)
                  C(0x55);                                          //11/11 push   %rbp
                  C(0x48);C(0x89);C(0xe5);                          //12/12 mov    %rsp,%rbp
                  C(0x41);C(0x52);                                  //15/15 push   %r10
                  C(0x48);C(0x81);C(0xec);C(0xb0);C(0x00);C(0x00);C(0x00);//17/17 sub    $0xb0,%rsp
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1);       //1e/1e vmovdqa in1(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//26/26 vmovaps %xmm0,-0xf0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+16);    //2e/2e vmovdqa in1+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x20);C(0xff);C(0xff);C(0xff);//36/36 vmovaps %xmm0,-0xe0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+32);    //3e/3e vmovdqa in1+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//46/46 vmovaps %xmm0,-0xd0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in1+48);    //4e/4e vmovdqa in1+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x40);C(0xff);C(0xff);C(0xff);//56/56 vmovaps %xmm0,-0xc0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2);       //5e/5e vmovdqa in2(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//66/66 vmovaps %xmm0,-0xb0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+16);    //6e/6e vmovdqa in2+16(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x60);C(0xff);C(0xff);C(0xff);//76/76 vmovaps %xmm0,-0xa0(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+32);    //7e/7e vmovdqa in2+32(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//86/86 vmovaps %xmm0,-0x90(%rbp)
                  C(0xc5);C(0xf9);C(0x6f);C(0x83);CI(64*in2+48);    //8e/8e vmovdqa in2+48(%rbx),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x45);C(0x80);          //96/96 vmovaps %xmm0,-0x80(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x10);C(0xff);C(0xff);C(0xff);//9b/9b vmovdqa -0xf0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0x85);C(0x50);C(0xff);C(0xff);C(0xff);//a3/a3 vpxor  -0xb0(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0xc1);                  //ab/ab vpxor  %ymm1,%ymm0,%ymm0
                  C(0xc5);C(0xf9);C(0x6f);C(0xd0);                  //af/af vmovdqa %xmm0,%xmm2
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xd0);C(0xfe);C(0xff);C(0xff);//b3/b3 vmovdqa %ymm0,-0x130(%rbp)
                  C(0xc5);C(0xfd);C(0x6f);C(0x85);C(0x30);C(0xff);C(0xff);C(0xff);//bb/bb vmovdqa -0xd0(%rbp),%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0x85);C(0x70);C(0xff);C(0xff);C(0xff);//c3/c3 vpxor  -0x90(%rbp),%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0xef);C(0xc1);                  //cb/cb vpxor  %ymm1,%ymm0,%ymm0
                  C(0xc5);C(0xfd);C(0x7f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//cf/cf vmovdqa %ymm0,-0x110(%rbp)
                  C(0xc5);C(0xf8);C(0x29);C(0x93);CI(64*out);       //d7/d7 vmovaps %xmm2,out(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xe0);C(0xfe);C(0xff);C(0xff);//df/df vmovdqa -0x120(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+16);    //e7/e7 vmovaps %xmm0,out+16(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0xf0);C(0xfe);C(0xff);C(0xff);//ef/ef vmovdqa -0x110(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+32);    //f7/f7 vmovaps %xmm0,out+32(%rbx)
                  C(0xc5);C(0xf9);C(0x6f);C(0x85);C(0x00);C(0xff);C(0xff);C(0xff);//ff/ff vmovdqa -0x100(%rbp),%xmm0
                  C(0xc5);C(0xf8);C(0x29);C(0x83);CI(64*out+48);    //107/107 vmovaps %xmm0,out+48(%rbx)
                  C(0x90);                                          //10f/10f nop
                  C(0xc5);C(0xf8);C(0x77);                          //110/110 vzeroupper
                  C(0x48);C(0x81);C(0xc4);C(0xb0);C(0x00);C(0x00);C(0x00);//113/113 add    $0xb0,%rsp
                  C(0x41);C(0x5a);                                  //11a/11a pop    %r10
                  C(0x5d);                                          //11c/11c pop    %rbp
                  C(0x49);C(0x8d);C(0x62);C(0xf8);                  //11d/11d lea    -0x8(%r10),%rsp
                  //code size: 289, stack requirements: 0
                  //registers: %rbx, %r10, %xmm2, %xmm0, %rbp, %ymm1, %ymm0, %rsp
                  break;
              default:
                  abort();
              }
        } 
    /// Restore modified registers
    C(0x5b);                                          //pop    %rbx
    C(0x41);C(0x5a);                                  //pop    %r10
    /// Return
    C(0xc3);
}

#if !(defined __LP64__ || defined __LLP64__) || defined _WIN32 && !defined _WIN64
#error "32-bit system is not supported"
#else
// we are compiling for a 64-bit system
#endif

// This file was generated at: http://www.fit.vutbr.cz/~vasicek/cgp Token:G1-RWYdgzx GCC:gcc-7.5
// Vasicek, Z., Slany, K.: Efficient Phenotype Evaluation in Cartesian Genetic Programming,
// Proc. of the 15th European Conference on Genetic Programming, EuroGP 2012

