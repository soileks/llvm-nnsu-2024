// RUN: split-file %s %t

// RUN: %clang_cc1 -load %llvmshlibdir/identifierRenamer%pluginext\
// RUN: -add-plugin identifierRenamer\
// RUN: -plugin-arg-identifierRenamer formerName=x\
// RUN: -plugin-arg-identifierRenamer renewedName=z %t/rename_var.cpp
// RUN: FileCheck %s < %t/rename_var.cpp --check-prefix=VAR

// VAR: int whoAreMe(int t) {
// VAR-NEXT: int z = 2, y = 3 + t;
// VAR-NEXT: z++;
// VAR-NEXT: y--;
// VAR-NEXT: return z + y;
// VAR-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/identifierRenamer%pluginext\
// RUN: -add-plugin identifierRenamer\
// RUN: -plugin-arg-identifierRenamer formerName=int\
// RUN: -plugin-arg-identifierRenamer renewedName=short %t/rename_type.cpp
// RUN: FileCheck %s < %t/rename_type.cpp --check-prefix=TYPE

// TYPE: short* whoAreMe(short x, short y) {
// TYPE-NEXT: short temp = x - y;
// TYPE-NEXT: short *result = &temp;
// TYPE-NEXT: return result;
// TYPE-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/identifierRenamer%pluginext\
// RUN: -add-plugin identifierRenamer\
// RUN: -plugin-arg-identifierRenamer formerName=whoAreMe\
// RUN: -plugin-arg-identifierRenamer renewedName=whoAreYou %t/rename_func.cpp
// RUN: FileCheck %s < %t/rename_func.cpp --check-prefix=FUNC

// FUNC: bool whoAreYou(bool isCorrect) {
// FUNC-NEXT: return isCorrect == true;
// FUNC-NEXT: }
// FUNC-NEXT: int whoAreMeOther(int id, int bal) {
// FUNC-NEXT: int check = whoAreYou(3) + whoAreYou(5);
// FUNC-NEXT: return check;
// FUNC-NEXT: }

// RUN: %clang_cc1 -load %llvmshlibdir/identifierRenamer%pluginext\
// RUN: -add-plugin identifierRenamer\
// RUN: -plugin-arg-identifierRenamer formerName=A\
// RUN: -plugin-arg-identifierRenamer renewedName=B %t/rename_class.cpp
// RUN: FileCheck %s < %t/rename_class.cpp --check-prefix=CLASS

// CLASS: class B {
// CLASS-NEXT: private:
// CLASS-NEXT: int a;
// CLASS-NEXT: public:
// CLASS-NEXT: B() {}
// CLASS-NEXT: ~B() {}
// CLASS-NEXT: void setA(int a) { this->a = a; }
// CLASS-NEXT: };
// CLASS: void H() {
// CLASS-NEXT: B a;
// CLASS-NEXT: a.setA(4);
// CLASS-NEXT: }
// CLASS: class C {
// CLASS-NEXT: private:
// CLASS-NEXT: int x, y;
// CLASS-NEXT: public:
// CLASS-NEXT: C() {}
// CLASS-NEXT: ~C() {}
// CLASS-NEXT: class B {
// CLASS-NEXT: private:
// CLASS-NEXT: int z;
// CLASS-NEXT: public:
// CLASS-NEXT: B() {}
// CLASS-NEXT: ~B() {}
// CLASS-NEXT: B(int z): z(z) {}
// CLASS-NEXT: B(B &other) {}
// CLASS-NEXT: B crazyFunction(B other) { return other; }
// CLASS-NEXT: int getZ() {
// CLASS-NEXT: return this->z;
// CLASS-NEXT: }
// CLASS-NEXT: class D {
// CLASS-NEXT: private:
// CLASS-NEXT: int t;
// CLASS-NEXT: public:
// CLASS-NEXT: D() {}
// CLASS-NEXT: ~D() {}
// CLASS-NEXT: void transit() {
// CLASS-NEXT: B apple;
// CLASS-NEXT: C *tinkoff = new C();
// CLASS-NEXT: delete tinkoff;
// CLASS-NEXT: B *oleg = new B();
// CLASS-NEXT: delete oleg;        
// CLASS-NEXT: }
// CLASS-NEXT: class E {
// CLASS-NEXT: private:
// CLASS-NEXT: int k;
// CLASS-NEXT: public:
// CLASS-NEXT: E() {}
// CLASS-NEXT: ~E() {}
// CLASS-NEXT: bool nikolaiSobolevHasGoneCrazy(int iq) {
// CLASS-NEXT: B yandex;
// CLASS-NEXT: D *is = new D();
// CLASS-NEXT: D very;
// CLASS-NEXT: delete is;
// CLASS-NEXT: B *good = new B();
// CLASS-NEXT: delete good;
// CLASS-NEXT: if (iq < 91) {
// CLASS-NEXT: return true;
// CLASS-NEXT: }
// CLASS-NEXT: return false;
// CLASS-NEXT: }
// CLASS-NEXT: class F {
// CLASS-NEXT: private:
// CLASS-NEXT: int m;
// CLASS-NEXT: public:
// CLASS-NEXT: F() {}
// CLASS-NEXT: ~F() {}
// CLASS-NEXT: int pressFToPayRespects(int check) {
// CLASS-NEXT: B kto_to;
// CLASS-NEXT: if (kto_to.getZ() >= 150) {
// CLASS-NEXT: return 15;
// CLASS-NEXT: }
// CLASS-NEXT: D one;
// CLASS-NEXT: return 10;
// CLASS-NEXT: }
// CLASS-NEXT: void singleFunction() {
// CLASS-NEXT: B mashina;
// CLASS-NEXT: B *vremeni = new B();
// CLASS-NEXT: B *sushestvuet = new B();
// CLASS-NEXT: delete vremeni;
// CLASS-NEXT: delete sushestvuet;
// CLASS-NEXT: }
// CLASS-NEXT: class I {
// CLASS-NEXT: private:
// CLASS-NEXT: int p;
// CLASS-NEXT: public:
// CLASS-NEXT: I() {}
// CLASS-NEXT: ~I() {}
// CLASS-NEXT: void goodGame() {
// CLASS-NEXT: B *spark = new B();
// CLASS-NEXT: B storm;
// CLASS-NEXT: B *madness = new B();
// CLASS-NEXT: delete madness;
// CLASS-NEXT: delete spark;
// CLASS-NEXT: E *four = new E();
// CLASS-NEXT: F three;
// CLASS-NEXT: delete four;
// CLASS-NEXT: }
// CLASS-NEXT: class P {
// CLASS-NEXT: private:
// CLASS-NEXT: int l;
// CLASS-NEXT: public:
// CLASS-NEXT: P() {}
// CLASS-NEXT: ~P() {}
// CLASS-NEXT: bool parents() {
// CLASS-NEXT: B love;
// CLASS-NEXT: B appreciate;
// CLASS-NEXT: C *two = new C();
// CLASS-NEXT: B respect;
// CLASS-NEXT: delete two;
// CLASS-NEXT: return true;
// CLASS-NEXT: }
// CLASS-NEXT: void reminder() {
// CLASS-NEXT: B *neverForgetAboutIt = new B();
// CLASS-NEXT: F *five = new F();
// CLASS-NEXT: delete neverForgetAboutIt;
// CLASS-NEXT: }
// CLASS-NEXT: };
// CLASS-NEXT: };
// CLASS-NEXT: };
// CLASS-NEXT: };  
// CLASS-NEXT: };
// CLASS-NEXT: };
// CLASS-NEXT: void function() {
// CLASS-NEXT: B okay;
// CLASS-NEXT: B *hooligan = new B(10);
// CLASS-NEXT: delete hooligan;
// CLASS-NEXT: }
// CLASS-NEXT: };

// RUN: %clang_cc1 -load %llvmshlibdir/identifierRenamer%pluginext\
// RUN: -add-plugin identifierRenamer\
// RUN: -plugin-arg-identifierRenamer otherFormerName=FormerName\
// RUN: -plugin-arg-identifierRenamer renewedName=B \
// RUN: 2>&1 | FileCheck %s --check-prefix=ERROR

// ERROR: Error: incorrect parameters input.

//--- rename_var.cpp
int whoAreMe(int t) {
  int x = 2, y = 3 + t;
  x++;
  y--;
  return x + y;
}
//--- rename_type.cpp
int* whoAreMe(int x, int y) {
  int temp = x - y;
  int *result = &temp;
  return result;
}
//--- rename_func.cpp
bool whoAreMe(bool isCorrect) {
  return isCorrect == true;
}
int whoAreMeOther(int id, int bal) {
  int check = whoAreMe(3) + whoAreMe(5);
  return check;
}
//--- rename_class.cpp
class A {
 private:
  int a;
 public:
  A() {}
  ~A() {}
  void setA(int a) { this->a = a; }
};

void H() {
  A a;
  a.setA(4);
}

class C {
 private:
  int x, y;
 public:
  C() {}
  ~C() {}
  class A {
   private:
     int z;
   public:
    A() {}
    ~A() {}
    A(int z): z(z) {}
    A(A &other) {}
    A crazyFunction(A other) { return other; }
    int getZ() {
      return this->z;
    }
    class D {
     private:
      int t;
     public:
      D() {}
      ~D() {}
      void transit() {
        A apple;
        C *tinkoff = new C();
        delete tinkoff;
        A *oleg = new A();
        delete oleg;
      }
      class E {
       private:
        int k;
       public:
        E() {}
        ~E() {}
        bool nikolaiSobolevHasGoneCrazy(int iq) {
          A yandex;
          D *is = new D();
          D very;
          delete is;
          A *good = new A();
          delete good;
          if (iq < 91) {
            return true;
          }
          return false;
        }
        class F {
         private:
          int m;
         public:
          F() {}
          ~F() {}
          int pressFToPayRespects(int check) {
            A kto_to;
            if (kto_to.getZ() >= 150) {
              return 15;
            }
            D one;
            return 10;
          }
          void singleFunction() {
            A mashina;
            A *vremeni = new A();
            A *sushestvuet = new A();
            delete vremeni;
            delete sushestvuet;
          }
          class I {
           private:
            int p;
           public:
            I() {}
            ~I() {}
            void goodGame() {
              A *spark = new A();
              A storm;
              A *madness = new A();
              delete madness;
              delete spark;
              E *four = new E();
              F three;
              delete four;
            }
           class P {
            private:
             int l;
            public:
             P() {}
             ~P() {}
             bool parents() {
               A love;
               A appreciate;
               C *two = new C();
               A respect;
               delete two;
               return true;
             }
             void reminder() {
              A *neverForgetAboutIt = new A();
              F *five = new F();
              delete neverForgetAboutIt;
             }
           };
          };
        };
      };  
    };
  };
  void function() {
    A okay;
    A *hooligan = new A(10);
    delete hooligan;
  }
};
