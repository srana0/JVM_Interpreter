public class DriverClass{
    public static void testClassWithReference(int paing, int pading,int padding,int paddding,ClassWithReference sample,int w,int x,int y){
        int z = 17;
        sample = new ClassWithReference();
        FourIntClass sample1 = new FourIntClass();
        sample1.a = w;
        sample1.b = x;
        sample1.c = y;
        sample1.d = z;
        sample.a = 42;
        sample.ref = sample1;
        int result = sample.ref.d+sample.ref.c - sample.ref.a - sample.ref.b - sample.a;
        System.out.println(result);
    }
    public static void testClassWithArrayReferences(int paing, int pading,int padding,int paddding,ClassWithArrayReferences sample,int w,int x,int y){
        int z = 19;
        sample = new ClassWithArrayReferences();
        ClassWithReference[] arrayReferences = new ClassWithReference[9];
        arrayReferences[6] = new ClassWithReference();
        arrayReferences[7] = new ClassWithReference();
        arrayReferences[8] = new ClassWithReference();
        FourIntClass sample1 = new FourIntClass();
        FourIntClass sample2 = new FourIntClass();
        FourIntClass sample3 = new FourIntClass();
        sample1.a = w;
        sample1.b = x;
        sample1.c = y;
        sample1.d = z;
        sample2.a = x;
        sample2.b = y;
        sample2.c = z;
        sample2.d = w;
        sample3.a = z;
        sample3.b = y;
        sample3.c = w;
        sample3.d = x;
        arrayReferences[6].ref = sample1;
        arrayReferences[7].ref = sample1;
        arrayReferences[8].ref = sample1;
        sample.array=arrayReferences;
        sample.number = 73;
        int result = 0;
        for (int i = 6 ; !(i - 8 == 0); i++){
            result = result + sample.number;
            result = result + sample.array[i].ref.a;
            result = result - sample.array[i].ref.b;
            result = result - sample.array[i].ref.c;
            result = result + sample.array[i].ref.d;
        }
        System.out.println(result);
    }
    public static void testClassWithIntArray(int paing, int pading,int padding,int paddding,ClassWithIntArray sample,int w,int x,int y){
        int z = 34;
        sample = new ClassWithIntArray();
        int[] fourthings = new int[10];
        fourthings[8] = w;
        fourthings[9] = y;
        fourthings[7] = z;
        fourthings[6] = x;
        sample.a = 61;
        sample.iarray = fourthings;
        FourIntClass sample1 = new FourIntClass();
        sample1.a = sample.iarray[6];
        sample1.b = sample.iarray[7];
        sample1.c = sample.iarray[8];
        sample1.d = sample.iarray[9];
        int result = (sample.iarray[7]-sample1.a+  sample.iarray[8]-sample1.b +   sample.iarray[6]-sample1.c - (  sample.iarray[9]-sample1.d ))*sample.a;
        System.out.println(result);
    }

    public static void testFourIntClass(int paing, int pading,int padding,int paddding,FourIntClass sample,int w,int x,int y){
        int z = 44;
        sample = new FourIntClass();
        sample.a = w;
        sample.b = x;
        sample.c = y;
        sample.d = z;
        System.out.println(sample.a);
        System.out.println(sample.b);
    }
    public static void main(String[] args){
        System.out.println(0);
        int paing; int pading;int padding;int paddding;
        FourIntClass sample = null;
        ClassWithReference sample1 = null;
        ClassWithIntArray sample2 = null;
        ClassWithArrayReferences sample3 = null;
        testFourIntClass(0,0,0,0,sample,16,23,12);
        if (sample != null){
            return;
        } else {
            System.out.println(3);
            testClassWithReference(0,0,0,0,sample1,42,7,17);
            if (sample1 != null){
                return;
            }else{
            System.out.println(17);
            testClassWithIntArray(0,0,0,0,sample2,25,64,80);
            if (sample2 != null){
                return;
            }else
            {
                System.out.println(-5);
                testClassWithArrayReferences(0,0,0,0,sample3,102,97,37);
                if (sample3 != null){
                    return;
                }
                else{
                      System.out.println(42);
                }
            }
            }
        }

    }
}