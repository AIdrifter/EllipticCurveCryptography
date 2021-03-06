#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openssl/e_os.h"
#include <openssl/opensslconf.h>	/* for OPENSSL_NO_ECDH */
#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/objects.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#ifdef OPENSSL_NO_ECDH
int main(int argc, char *argv[])
{
    printf("No ECDH support\n");
    return(0);
}
#else
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#ifdef OPENSSL_SYS_WIN16
#define MS_CALLBACK	_far _loadds
#else
#define MS_CALLBACK
#endif

#if 0
static void MS_CALLBACK cb(int p, int n, void *arg);
#endif

static const char rnd_seed[] = "string to make the random number generator think it has entropy";


static const int KDF1_SHA1_len = 20;
static void *KDF1_SHA1(const void *in, size_t inlen,   void *out,              size_t *outlen)

	{
#ifndef OPENSSL_NO_SHA
	if (*outlen < SHA_DIGEST_LENGTH)
		return NULL;
	else
		*outlen = SHA_DIGEST_LENGTH;
	return SHA1(in, inlen, out);
#else
	return NULL;
#endif
	}



int main(int argc, char *argv[])
{
	BN_CTX *ctx=NULL;
	int nid;
	BIO *out;
	CRYPTO_malloc_debug_init();
	CRYPTO_dbg_set_options(V_CRYPTO_MDEBUG_ALL);
	CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
	const char *text = "NIST Prime-Curve P-192";

#ifdef OPENSSL_SYS_WIN32
	CRYPTO_malloc_init();
#endif

	RAND_seed(rnd_seed, sizeof rnd_seed);
	out=BIO_new(BIO_s_file());
	if (out == NULL) EXIT(1);
	BIO_set_fp(out,stdout,BIO_NOCLOSE);

	if ((ctx=BN_CTX_new()) == NULL) goto err;
	nid = NID_X9_62_prime192v1;

printf("in ecdh test\n");
	EC_KEY *a=NULL;    //EC_KEY is a structure
	EC_KEY *b=NULL;
	BIGNUM *x_a=NULL, *y_a=NULL,
	       *x_b=NULL, *y_b=NULL;
	char buf[12];
	unsigned char *abuf=NULL,*bbuf=NULL;
	int i,alen,blen,aout,bout,ret=0;
	const EC_GROUP *group;

	a = EC_KEY_new_by_curve_name(nid);
	b = EC_KEY_new_by_curve_name(nid);
	if (a == NULL || b == NULL)
		goto err;

	group = EC_KEY_get0_group(a); //returns the EC_GROUP structure created by the EC_KEY structure
//EC_GROUP structure is present in the EC_KEY structure.

	if ((x_a=BN_new()) == NULL) goto err;    //BN_new returns a pointer to the bignum
	if ((y_a=BN_new()) == NULL) goto err;
	if ((x_b=BN_new()) == NULL) goto err;
	if ((y_b=BN_new()) == NULL) goto err;

	BIO_puts(out,"Testing key generation with ");
	BIO_puts(out,text);

#ifdef NOISY
	printf ("noisy");
	BIO_puts(out,"\n");
	BIO_puts(out,"\n");
	BIO_puts(out,"\n");
#else
	BIO_flush(out);
#endif


	if (!EC_KEY_generate_key(a)) goto err;
	printf("\n1 ) generating keys\n");

	if (EC_METHOD_get_field_type(EC_GROUP_method_of(group)) == NID_X9_62_prime_field)
		{
		if (!EC_POINT_get_affine_coordinates_GFp(group,EC_KEY_get0_public_key(a), x_a, y_a, ctx)) goto err;
		}
                                                                  //returns the public key
	else
		{
		if (!EC_POINT_get_affine_coordinates_GF2m(group,EC_KEY_get0_public_key(a), x_a, y_a, ctx)) goto err;
		}

	//BN_print_fp(stdout, a->pub_key);
	//printf("private key is : ");
	//BN_print_fp(stdout, EC_KEY_get0_private_key(a));
	//printf("\nAffine cordinates x:");
	//BN_print_fp(stdout, x_a);
	//printf("\nAffine cordinates y:");
	//BN_print_fp(stdout, y_a);

	BIO_puts(out,"  pri 1=");
	BN_print(out,EC_KEY_get0_private_key(a));
	BIO_puts(out,"\n  pub 1=");
	BN_print(out,x_a);
	BIO_puts(out,",");
	BN_print(out,y_a);
	BIO_puts(out,"\n");

printf("\n2 ) generated keys , generated affine points x and y , and also determided the primse brinary case\n");

#ifdef NOISY
	printf("no generation");
	BIO_puts(out,"  pri 1=");
	BN_print(out,a->priv_key);
	BIO_puts(out,"\n  pub 1=");
	BN_print(out,x_a);
	BIO_puts(out,",");
	BN_print(out,y_a);
	BIO_puts(out,"\n");
#else
	BIO_printf(out," .");
	BIO_flush(out);
#endif

//public key number two is created here

	if (!EC_KEY_generate_key(b)) goto err;

	if (EC_METHOD_get_field_type(EC_GROUP_method_of(group)) == NID_X9_62_prime_field)
		{
		if (!EC_POINT_get_affine_coordinates_GFp(group,
			EC_KEY_get0_public_key(b), x_b, y_b, ctx)) goto err;    // not well
		}
	else
		{
		if (!EC_POINT_get_affine_coordinates_GF2m(group,
			EC_KEY_get0_public_key(b), x_b, y_b, ctx)) goto err;
		}
//		printf("this program is veyr irritating%d", sizeof(*EC_POINT));

printf("\n******************************************************************************\n");
//	printf("public key is : ");
//	BN_print_fp(stdout, EC_KEY_get0_private_key(b));

	BIO_puts(out,"  pri 2=");
	BN_print(out,EC_KEY_get0_private_key(b));
	BIO_puts(out,"\n  pub 2=");
	BN_print(out,x_b);
	BIO_puts(out,",");
	BN_print(out,y_b);
	BIO_puts(out,"\n");


#ifdef NOISY
	BIO_puts(out,"  pri 2=");
	BN_print(out,b->priv_key);
	BIO_puts(out,"\n  pub 2=");
	BN_print(out,x_b);
	BIO_puts(out,",");
	BN_print(out,y_b);
	BIO_puts(out,"\n");
#else
	BIO_printf(out,".");
	BIO_flush(out);
#endif



	alen=KDF1_SHA1_len; ///it is a static constant integer.
	abuf=(unsigned char *)OPENSSL_malloc(alen);
	aout=ECDH_compute_key(abuf,alen,EC_KEY_get0_public_key(b),a,KDF1_SHA1); //generating session key
  //      BN_print(out, abuf);
	//BIO_puts(out,"\n");

	BIO_puts(out,"  key1 =");
	for (i=0; i<aout; i++)
		{
		sprintf(buf,"%02X",abuf[i]);
		BIO_puts(out,buf);
		}
	BIO_puts(out,"\n");


#ifdef NOISY
	BIO_puts(out,"  key1 =");
	for (i=0; i<aout; i++)
		{
		sprintf(buf,"%02X",abuf[i]);
		BIO_puts(out,buf);
		}
	BIO_puts(out,"\n");
#else
	BIO_printf(out,".");
	BIO_flush(out);
#endif



	blen=KDF1_SHA1_len;
	bbuf=(unsigned char *)OPENSSL_malloc(blen);
	bout=ECDH_compute_key(bbuf,blen,EC_KEY_get0_public_key(a),b,KDF1_SHA1);
//	BN_print(out, bbuf);
//	BIO_puts(out,"\n");

	BIO_puts(out,"  key2 =");
	for (i=0; i<bout; i++)
		{
		sprintf(buf,"%02X",bbuf[i]);
		BIO_puts(out,buf);
		}
	BIO_puts(out,"\n");


#ifdef NOISY
	BIO_puts(out,"  key2 =");
	for (i=0; i<bout; i++)
		{
		sprintf(buf,"%02X",bbuf[i]);
		BIO_puts(out,buf);
		}
	BIO_puts(out,"\n");
#else
	BIO_printf(out,".");
	BIO_flush(out);
#endif

printf("\n**************************************Final REsults**********************************************\n");
	if ((aout < 4) || (bout != aout) || (memcmp(abuf,bbuf,aout) != 0))
		{
		BIO_printf(out, " failed\n\n");
		BIO_printf(out, "key a:\n");
		BIO_printf(out, "private key: ");
		BN_print(out, EC_KEY_get0_private_key(a));
		BIO_printf(out, "\n");
		BIO_printf(out, "public key (x,y): ");
		BN_print(out, x_a);
		BIO_printf(out, ",");
		BN_print(out, y_a);
		BIO_printf(out, "\nkey b:\n");
		BIO_printf(out, "private key: ");
		BN_print(out, EC_KEY_get0_private_key(b));
		BIO_printf(out, "\n");
		BIO_printf(out, "public key (x,y): ");
		BN_print(out, x_b);
		BIO_printf(out, ",");
		BN_print(out, y_b);
		BIO_printf(out, "\n");
		BIO_printf(out, "generated key a: ");
		for (i=0; i<bout; i++)
			{
			sprintf(buf, "%02X", bbuf[i]);
			BIO_puts(out, buf);
			}
		BIO_printf(out, "\n");
		BIO_printf(out, "generated key b: ");
		for (i=0; i<aout; i++)
			{
			sprintf(buf, "%02X", abuf[i]);
			BIO_puts(out,buf);
			}



#ifndef NOISY
		BIO_printf(out, " failed\n\n");
		BIO_printf(out, "key a:\n");
		BIO_printf(out, "private key: ");
		BN_print(out, EC_KEY_get0_private_key(a));
		BIO_printf(out, "\n");
		BIO_printf(out, "public key (x,y): ");
		BN_print(out, x_a);
		BIO_printf(out, ",");
		BN_print(out, y_a);
		BIO_printf(out, "\nkey b:\n");
		BIO_printf(out, "private key: ");
		BN_print(out, EC_KEY_get0_private_key(b));
		BIO_printf(out, "\n");
		BIO_printf(out, "public key (x,y): ");
		BN_print(out, x_b);
		BIO_printf(out, ",");
		BN_print(out, y_b);
		BIO_printf(out, "\n");
		BIO_printf(out, "generated key a: ");
		for (i=0; i<bout; i++)
			{
			sprintf(buf, "%02X", bbuf[i]);
			BIO_puts(out, buf);
			}
		BIO_printf(out, "\n");
		BIO_printf(out, "generated key b: ");
		for (i=0; i<aout; i++)
			{
			sprintf(buf, "%02X", abuf[i]);
			BIO_puts(out,buf);
			}
		BIO_printf(out, "\n");
#endif
		fprintf(stderr,"Error in ECDH routines\n");
		ret=0;
		}
	else
		{
#ifndef NOISY
		BIO_printf(out, " ok\n");
#endif
		ret=1;
		}
err:
	ERR_print_errors_fp(stderr);

	if (abuf != NULL) OPENSSL_free(abuf);
	if (bbuf != NULL) OPENSSL_free(bbuf);
	if (x_a) BN_free(x_a);
	if (y_a) BN_free(y_a);
	if (x_b) BN_free(x_b);
	if (y_b) BN_free(y_b);
	if (b) EC_KEY_free(b);
	if (a) EC_KEY_free(a);
return 0;

}
#endif


