//#################################################################################################
//
// This is a proof-of-concept implementation of the Proof-of-Bit-Length algorithm, as originally 
// described in the First International Workshop on Practice and Theory in Public Key Cryptography 
// PKC '98 paper:
//
//	 "Guaranteed Correct Sharing of Integer Factorization with Off-Line Shareholders"
//
// This implementation was written by *** *** [mail at *** at *** dot com] in 2021. This 
// Range proof algorithm is a base-3 application of original base-2 construction, introduced by Wenbo 
// Mao at PKC '98. 
//
// This code is released under the GNU General Public License v3.0 license (see https://opensource.org/licenses/GPL-3.0).
//
//#################################################################################################

#include <iostream>
#include <mpirxx.h>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <stdint.h>

using namespace std;

void syspause() {
    std::cout << "\nPress Enter To Continue..." << std::endl;
    std::cin.clear();
    std::cin.ignore();
    std::cin.get();

}

//########################################
//  		GLOBAL PARAMETERS
//########################################
// We work over the ring of integers Z. The parameters p and q are two large primes.
// g is an element of order q. f is a fixed element generated by pseudo random generator seeded by g.

// 1024-bit parameter:

mpz_class p("43323684510717572313232727444172291314491824716292944791480224212865535266619992174970805232963927127347884580175130419569925799009301231875061504134839721231460111692155701447535444517221040879619270994076956689376073310086388688215318320437086614069950987525274166972174985903270731655835827418424034671431075213970042381160234909372115351052737");
mpz_class q("1613694906937332210711275437259789840715630336384486051099417535112369216461200184420207000092971");
mpz_class g("25154361211065433107857466046223821329020294681814436680019203867970290067733882360070801069946593853390592996716433213159956013105174207358819231039371855400327588080590753844598772506822114498049974021792421771846060176038318061197855549443172131620551196591615090227369082463713704543690773183685935934786126990809205360041529076701453953236639");
mpz_class f("14985298546741725701394984213910451487471377421786342751549317436811893077294093928319409654230462351306432467512238342520562601169236950824217249693564776618766474897262030359380674885847572315819025487023377287781285250576400101019608855386334602804948716077036235894587380829015180443531501272804025356442485247035026274675598397060600015606200");


/*
// Here one can found different bit size parameter choices.


// 512-bit parameter:

mpz_class p("13232376895198612407547930718267435757728527029623408872245156039757713029036368719146452186041204237350521785240337048752071462798273003935646236777459223");
mpz_class q("857393771208094202104259627990318636601332086981");
mpz_class g("5421644057436475141609648488325705128047428394380474376834667300766108262613900542681289080713724597310673074119355136085795982097390670890367185141189796");
mpz_class f("10783827985936883407800478884376885258012329124816552994400318669417122279843086645137200743427232531167766104260606805303022314906254403593803159583034340");


*/


//###############################################################################
// The verification part with 3 items
int Verification(mpz_class E, mpz_class a_1, mpz_class a_2, mpz_class c, mpz_class c_1, mpz_class c_2, mpz_class r_1, mpz_class r_2) {
    mpz_class lhs = 0;
    int count = 0;
    //The first check of verification
    if (c == (c_1 + c_2) % q)  count++;

    mpz_class half1; mpz_class half2;                                               // Some tempotary values to store short time values
    mpz_class temp;

    //The second check of verification
    mpz_invert(temp.get_mpz_t(), g.get_mpz_t(), p.get_mpz_t());
    temp = (temp * E) % p;
    mpz_powm(temp.get_mpz_t(), temp.get_mpz_t(), c_1.get_mpz_t(), p.get_mpz_t());
    temp = (temp * a_2) % p;
    mpz_powm(lhs.get_mpz_t(), f.get_mpz_t(), r_1.get_mpz_t(), p.get_mpz_t());

    if (lhs == temp) count++;                                                       // The case of f^{r1}=a_2(E/g)^{c_1} mod p

    //The third check of verification
    mpz_powm(lhs.get_mpz_t(), f.get_mpz_t(), r_2.get_mpz_t(), p.get_mpz_t());
    mpz_powm(temp.get_mpz_t(), E.get_mpz_t(), c_2.get_mpz_t(), p.get_mpz_t());
    temp = (temp * a_1) % p;

    if (lhs == temp) count++;                                                       // The case of f^{r2}=a_1(E)^{c_2} mod p

    if (count == 3) {
        return 1;
    }
    else
        return 0;



}

//###############################################################################
//###############################################################################
//###############################################################################

// The main execution of the base-2 Range Proof algorithm
// Very important note: Call for verification parts made as comment, since we
// want to measure the computation cost of the proovers part. If you want to
// fully use this implementation please put the verification calls out of comments.

int main() {

    std::clock_t start;
    double duration;
    start = std::clock();

    double ortalama = 0;

    gmp_randstate_t random;
    gmp_randinit_default(random);
    gmp_randseed_ui(random, time(0));

    mpz_class inv_g;
    mpz_invert(inv_g.get_mpz_t(), g.get_mpz_t(), p.get_mpz_t());                // We take the inverse of g at the beginning.

    // To take the average computation cost decide how many time you want to call this proof (i:=times)
    for (int i = 0; i < 1000; i++) {


        mpz_class z;
        mpz_class temp2;
        mpz_class w;
        mpz_class r_1; mpz_class r_2;
        mpz_class c_1; mpz_class c_2;
        mpz_class a_1; mpz_class a_2;
        mpz_class c;
        mpz_class E = 0;

        int boolean;
        boolean = 0;

        int Result[128] = {};
        int rslt;
        // Decide which number you want to check msg:secret message
        mpz_class msg("767567856487547");


        unsigned long BitLength = 0;
        BitLength = mpz_sizeinbase(msg.get_mpz_t(), 2);


        mpz_class half1 = 0;  mpz_class half2;
        mpz_class half21;  mpz_class half22;


        mpz_class* arr_Ei_values = new mpz_class[1024];
        mpz_class* arr_z_values = new mpz_class[1024];
        mpz_class t = 0;
        for (int i = 0; i < BitLength; ++i) {


            // random z is generated in each step, we store z values in an array
           
                mpz_urandomm(arr_z_values[i].get_mpz_t(), random, q.get_mpz_t());
           
                t += (arr_z_values[i] * pow(2, i)) % q;

        }
        //###############################################################################
        // For each bit of the message we call the following OR-proof
        //###############################################################################
        for (mp_bitcnt_t BitIndex = 0; BitIndex < BitLength; ++BitIndex)
        {
            mpz_class n_th_bit;
            n_th_bit = mpz_tstbit(msg.get_mpz_t(), BitIndex);

            z = arr_z_values[BitIndex];

            //###############################################################################
            // 1. This is the case of n'th bit of the message equals to 0
            //###############################################################################
            if (n_th_bit == 0) {
                mpz_powm(E.get_mpz_t(), f.get_mpz_t(), z.get_mpz_t(), p.get_mpz_t());           // Compute the commitment
                arr_Ei_values[BitIndex] = E;                                                    // Store the commitment in an array to use in verification afterward

                // Random parameters are computed in the following 

                    mpz_urandomm(w.get_mpz_t(), random, q.get_mpz_t());
              
                    mpz_urandomm(r_1.get_mpz_t(), random, q.get_mpz_t());
                          
                    mpz_urandomm(c_1.get_mpz_t(), random, q.get_mpz_t());
              

                //Computing a1 and a2
                mpz_powm(a_1.get_mpz_t(), f.get_mpz_t(), w.get_mpz_t(), p.get_mpz_t());                 // a1=f^{w} mod p


                mpz_powm(half1.get_mpz_t(), f.get_mpz_t(), r_1.get_mpz_t(), p.get_mpz_t());
                temp2 = (E * inv_g) % p;
                mpz_class minus_c1 = -c_1;
                mpz_powm(half2.get_mpz_t(), temp2.get_mpz_t(), minus_c1.get_mpz_t(), p.get_mpz_t());
                a_2 = (half1 * half2) % p;                                                              // a2=f^{r_1}(E/g)^{-c_1} mod p


                // Random c selection is computed by the verifier. We put it here bc it will not affect the cost significantly.
          
                    mpz_urandomm(c.get_mpz_t(), random, q.get_mpz_t());
        

                // Some prover part computations:

                c_2 = (c - c_1);
                if (c_2 < 0) { c_2 += q; }

                r_2 = (w + (z * c_2)) % q;

                // THIS IS THE CALL FOR VERIFICATION PART
               /*  rslt = Verification(E, a_1, a_2, c, c_1, c_2, r_1, r_2);
                   Result[BitIndex] = rslt;
                   std::cout << "Index: " << BitIndex << " Index term: " << Result[BitIndex] << endl;  */
            }

            //###############################################################################
            // 2. This is the case of n'th bit of the message equals to 1
            //###############################################################################
            else if (n_th_bit == 1) {

                mpz_powm(E.get_mpz_t(), f.get_mpz_t(), z.get_mpz_t(), p.get_mpz_t());
                mpz_mul(E.get_mpz_t(), g.get_mpz_t(), E.get_mpz_t());                       //Compute the commitment

                if (E < 0)
                {
                    __debugbreak();
                }

                mpz_mod(E.get_mpz_t(), E.get_mpz_t(), p.get_mpz_t());
                arr_Ei_values[BitIndex] = E;                                                // Store the commitment in an array to use in verification afterward

                // Random parameters are computed in the following.
               
                    mpz_urandomm(w.get_mpz_t(), random, q.get_mpz_t());
                                
                    mpz_urandomm(r_2.get_mpz_t(), random, q.get_mpz_t());
                               
                    mpz_urandomm(c_2.get_mpz_t(), random, q.get_mpz_t());
               


                // Computing a1 and a2
                mpz_powm(half1.get_mpz_t(), f.get_mpz_t(), r_2.get_mpz_t(), p.get_mpz_t());
                mpz_class minus_c_2 = -c_2;
                mpz_powm(half2.get_mpz_t(), E.get_mpz_t(), minus_c_2.get_mpz_t(), p.get_mpz_t());
                a_1 = (half1 * half2) % p;                                                              // a1=f^{r_2}E^{-c_2}  mod p


                mpz_powm(a_2.get_mpz_t(), f.get_mpz_t(), w.get_mpz_t(), p.get_mpz_t());                 // a2=f^{w}  mod p

                // Random c selection is computed by the verifier.
               
                    mpz_urandomm(c.get_mpz_t(), random, q.get_mpz_t());
                
                // Some prover part computations:

                c_1 = (c - c_2);
                if (c_1 < 0) { c_1 += q; }

                r_1 = (w + (z * c_1)) % q;

                // THIS IS THE CALL FOR VERIFICATION PART
                /* rslt = Verification(E, a_1, a_2, c, c_1, c_2, r_1, r_2);
                   Result[BitIndex] = rslt;
                   std::cout << "Index: " << BitIndex << " Index term: " << Result[BitIndex] << endl;  */
            }

            else {
                std::cout << "bit is not 0 nor 1, error";
            }

        }


        duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
        //       std::cout << "Time interval: " << duration << '\n';

        ortalama += duration;

        //###############################################################################
        // 3. Using homomorphism property, we check the accuracy of last condition
        //###############################################################################

   /*     mpz_class lhs;
        mpz_powm(half1.get_mpz_t(), g.get_mpz_t(), msg.get_mpz_t(), p.get_mpz_t());
        mpz_powm(half2.get_mpz_t(), f.get_mpz_t(), t.get_mpz_t(), p.get_mpz_t());
        lhs = (half1 * half2) % p;
          std::cout << "lhs " << lhs << endl;
        mpz_class temp1, temp3 = 1;
        for (int j = 0; j < BitLength; ++j) {
            temp1 = pow(2, j);

            mpz_powm(temp2.get_mpz_t(), arr_Ei_values[j].get_mpz_t(), temp1.get_mpz_t(), p.get_mpz_t());

            temp3 = (temp3 * temp2) % p;

        }
          std::cout << "rhs " << temp3 << '\n';  */

        delete[] arr_Ei_values;
        delete[] arr_z_values;
    }

    ortalama = ortalama / 1000;
    std::cout << "ORTALAMA Time interval: " << ortalama << '\n';

    syspause();

}