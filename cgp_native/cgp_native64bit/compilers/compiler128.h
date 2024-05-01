// typedef int nodetype;
// nodetype* nodeoutput;

void cgp_init_consts() {}

#define MAXCODESIZE 20
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
    /// %edx, %rbx, %eax
    C(0x53);                                          //push   %rbx
    C(0x52);                                          //push   %rdx
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
                  C(0x8b);C(0x93);CI(4*in1);                        //0/0 mov    in1(%rbx),%edx
                  C(0x89);C(0x93);CI(4*out);                        //6/6 mov    %edx,out(%rbx)
                  //code size: 12, stack requirements: 0
                  //registers: %edx, %rbx
                  break;
              case 1:
                  ///nodeoutput[out] = ~nodeoutput[in1];
                  C(0x8b);C(0x83);CI(4*in1);                        //0/0 mov    in1(%rbx),%eax
                  C(0xf7);C(0xd0);                                  //6/6 not    %eax
                  C(0x89);C(0x83);CI(4*out);                        //8/8 mov    %eax,out(%rbx)
                  //code size: 14, stack requirements: 0
                  //registers: %eax, %rbx
                  break;
              case 2:
                  ///nodeoutput[out] = nodeoutput[in1] & nodeoutput[in2];
                  C(0x8b);C(0x93);CI(4*in1);                        //0/0 mov    in1(%rbx),%edx
                  C(0x23);C(0x93);CI(4*in2);                        //6/6 and    in2(%rbx),%edx
                  C(0x89);C(0x93);CI(4*out);                        //c/c mov    %edx,out(%rbx)
                  //code size: 18, stack requirements: 0
                  //registers: %edx, %rbx
                  break;
              case 3:
                  ///nodeoutput[out] = nodeoutput[in1] | nodeoutput[in2];
                  C(0x8b);C(0x93);CI(4*in1);                        //0/0 mov    in1(%rbx),%edx
                  C(0x0b);C(0x93);CI(4*in2);                        //6/6 or     in2(%rbx),%edx
                  C(0x89);C(0x93);CI(4*out);                        //c/c mov    %edx,out(%rbx)
                  //code size: 18, stack requirements: 0
                  //registers: %edx, %rbx
                  break;
              case 4:
                  ///nodeoutput[out] = nodeoutput[in1] ^ nodeoutput[in2];
                  C(0x8b);C(0x93);CI(4*in1);                        //0/0 mov    in1(%rbx),%edx
                  C(0x33);C(0x93);CI(4*in2);                        //6/6 xor    in2(%rbx),%edx
                  C(0x89);C(0x93);CI(4*out);                        //c/c mov    %edx,out(%rbx)
                  //code size: 18, stack requirements: 0
                  //registers: %edx, %rbx
                  break;
              case 5:
                  ///nodeoutput[out] = ~(nodeoutput[in1] & nodeoutput[in2]);
                  C(0x8b);C(0x83);CI(4*in1);                        //0/0 mov    in1(%rbx),%eax
                  C(0x23);C(0x83);CI(4*in2);                        //6/6 and    in2(%rbx),%eax
                  C(0xf7);C(0xd0);                                  //c/c not    %eax
                  C(0x89);C(0x83);CI(4*out);                        //e/e mov    %eax,out(%rbx)
                  //code size: 20, stack requirements: 0
                  //registers: %eax, %rbx
                  break;
              case 6:
                  ///nodeoutput[out] = ~(nodeoutput[in1] | nodeoutput[in2]);
                  C(0x8b);C(0x83);CI(4*in1);                        //0/0 mov    in1(%rbx),%eax
                  C(0x0b);C(0x83);CI(4*in2);                        //6/6 or     in2(%rbx),%eax
                  C(0xf7);C(0xd0);                                  //c/c not    %eax
                  C(0x89);C(0x83);CI(4*out);                        //e/e mov    %eax,out(%rbx)
                  //code size: 20, stack requirements: 0
                  //registers: %eax, %rbx
                  break;
              case 7:
                  ///nodeoutput[out] = ~(nodeoutput[in1] ^ nodeoutput[in2]);
                  C(0x8b);C(0x83);CI(4*in1);                        //0/0 mov    in1(%rbx),%eax
                  C(0x33);C(0x83);CI(4*in2);                        //6/6 xor    in2(%rbx),%eax
                  C(0xf7);C(0xd0);                                  //c/c not    %eax
                  C(0x89);C(0x83);CI(4*out);                        //e/e mov    %eax,out(%rbx)
                  //code size: 20, stack requirements: 0
                  //registers: %eax, %rbx
                  break;
              default:
                  abort();
              }
        } 
    /// Restore modified registers
    C(0x5a);                                          //pop    %rdx
    C(0x5b);                                          //pop    %rbx
    /// Return
    C(0xc3);
}

#if !(defined __LP64__ || defined __LLP64__) || defined _WIN32 && !defined _WIN64
#error "32-bit system is not supported"
#else
// we are compiling for a 64-bit system
#endif

// This file was generated at: http://www.fit.vutbr.cz/~vasicek/cgp Token:G1-R2BMYgx GCC:gcc-7.5
// Vasicek, Z., Slany, K.: Efficient Phenotype Evaluation in Cartesian Genetic Programming,
// Proc. of the 15th European Conference on Genetic Programming, EuroGP 2012

