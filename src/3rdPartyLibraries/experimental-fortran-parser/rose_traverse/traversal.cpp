#include "traversal.h"
#include "OFPNodes.h"
#include <string.h>

#define DEBUG_PRINT
#undef  DEBUG_OFP_CLIENT


/**
 * Perform any necessary initialization for this traversal
 */
ATbool ofp_traverse_init()
{
   return ATtrue;
}

static char * ofp_strdup(char * str)
{
   return strdup(str);
}

ATbool ofp_traverse_Dop(ATerm term, OFP::Dop* Dop)
{
#ifdef DEBUG_PRINT
   printf("Dop(F): %s\n", ATwriteToString(term));
#endif

   char * Dop_val;
   if (ATmatch(term, "<str>", &Dop_val)) {
      // MATCHED Dop
      Dop->setValue(Dop_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_HexConstant(ATerm term, OFP::HexConstant* HexConstant)
{
#ifdef DEBUG_PRINT
   printf("HexConstant(F): %s\n", ATwriteToString(term));
#endif

   char * HexConstant_val;
   if (ATmatch(term, "<str>", &HexConstant_val)) {
      // MATCHED HexConstant
      HexConstant->setValue(HexConstant_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_OctalConstant(ATerm term, OFP::OctalConstant* OctalConstant)
{
#ifdef DEBUG_PRINT
   printf("OctalConstant(F): %s\n", ATwriteToString(term));
#endif

   char * OctalConstant_val;
   if (ATmatch(term, "<str>", &OctalConstant_val)) {
      // MATCHED OctalConstant
      OctalConstant->setValue(OctalConstant_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_BinaryConstant(ATerm term, OFP::BinaryConstant* BinaryConstant)
{
#ifdef DEBUG_PRINT
   printf("BinaryConstant(F): %s\n", ATwriteToString(term));
#endif

   char * BinaryConstant_val;
   if (ATmatch(term, "<str>", &BinaryConstant_val)) {
      // MATCHED BinaryConstant
      BinaryConstant->setValue(BinaryConstant_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_Rcon(ATerm term, OFP::Rcon* Rcon)
{
#ifdef DEBUG_PRINT
   printf("Rcon(F): %s\n", ATwriteToString(term));
#endif

   char * Rcon_val;
   if (ATmatch(term, "<str>", &Rcon_val)) {
      // MATCHED Rcon
      Rcon->setValue(Rcon_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_Scon(ATerm term, OFP::Scon* Scon)
{
#ifdef DEBUG_PRINT
   printf("Scon(F): %s\n", ATwriteToString(term));
#endif

   char * Scon_val;
   if (ATmatch(term, "<str>", &Scon_val)) {
      // MATCHED Scon
      Scon->setValue(Scon_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_Icon(ATerm term, OFP::Icon* Icon)
{
#ifdef DEBUG_PRINT
   printf("Icon(F): %s\n", ATwriteToString(term));
#endif

   char * Icon_val;
   if (ATmatch(term, "<str>", &Icon_val)) {
      // MATCHED Icon
      Icon->setValue(Icon_val);

#ifdef OFP_CLIENT
      //TODO-CER-2014.3.7 set (or request) SgToken::FORTRAN_INTEGER);
      SgUntypedType*             type = new SgUntypedType(NULL, "INTEGER");
      //TODO-CER-2014.3.7 set (or request) SgToken::FORTRAN_INTEGER);
      SgUntypedValueExpression* value = new SgUntypedValueExpression(NULL, SgToken::FORTRAN_UNKNOWN, Icon->getValue(), type);
      Icon->setPayload(value);
#ifdef DEBUG_OFP_CLIENT
      printf("ROSE Icon: .......................... ");
      unparser->unparseExpr(dynamic_cast<SgUntypedExpression*>(Icon->getPayload()));  printf("\n");
#endif
#endif

      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_Ident(ATerm term, OFP::Ident* Ident)
{
#ifdef DEBUG_PRINT
   printf("Ident(F): %s\n", ATwriteToString(term));
#endif

   char * Ident_val;
   if (ATmatch(term, "<str>", &Ident_val)) {
      // MATCHED Ident
      Ident->setValue(Ident_val);

#ifdef OFP_CLIENT 
      //TODO-CER-2014.3.7 set (or request) SgToken::FORTRAN_INTEGER);
      SgUntypedValueExpression* value = new SgUntypedValueExpression(NULL, SgToken::FORTRAN_INTEGER, Ident->getValue(), NULL);
      Ident->setPayload(value);
#ifdef DEBUG_OFP_CLIENT
      printf("ROSE Ident: ......................... ");
      unparser->unparseExpr(dynamic_cast<SgUntypedExpression*>(Ident->getPayload()));  printf("\n");
#endif
#endif

      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_Letter(ATerm term, OFP::Letter* Letter)
{
#ifdef DEBUG_PRINT
   printf("Letter(F): %s\n", ATwriteToString(term));
#endif

   char * Letter_val;
   if (ATmatch(term, "<str>", &Letter_val)) {
      // MATCHED Letter
      Letter->setValue(Letter_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_Label(ATerm term, OFP::Label* Label)
{
#ifdef DEBUG_PRINT
   printf("Label(F): %s\n", ATwriteToString(term));
#endif

   char * Label_val;
   if (ATmatch(term, "<str>", &Label_val)) {
      // MATCHED Label
      Label->setValue(Label_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_LblRef(ATerm term, OFP::LblRef* LblRef)
{
#ifdef DEBUG_PRINT
   printf("LblRef(F): %s\n", ATwriteToString(term));
#endif

   char * LblRef_val;
   if (ATmatch(term, "<str>", &LblRef_val)) {
      // MATCHED LblRef
      LblRef->setValue(LblRef_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_StartCommentBlock(ATerm term, OFP::StartCommentBlock* StartCommentBlock)
{
#ifdef DEBUG_PRINT
   printf("StartCommentBlock(F): %s\n", ATwriteToString(term));
#endif

   char * StartCommentBlock_val;
   if (ATmatch(term, "<str>", &StartCommentBlock_val)) {
      // MATCHED StartCommentBlock
      StartCommentBlock->setValue(StartCommentBlock_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}

ATbool ofp_traverse_EOS(ATerm term, OFP::EOS* EOS)
{
#ifdef DEBUG_PRINT
   printf("EOS(F): %s\n", ATwriteToString(term));
#endif

   char * EOS_val;
   if (ATmatch(term, "<str>", &EOS_val)) {
      // MATCHED EOS
      EOS->setValue(EOS_val);
      return ATtrue;
   } else return ATfalse;

   return ATtrue;
}
