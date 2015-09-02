/* Name: Andrej Ivanovic	

   UID: 604016344

   Others With Whom I Discussed Things: 

   Other Resources I Consulted: solution for python version of gaussian blurr
   
*/

import java.io.*;
import java.util.*;
import java.util.stream.IntStream;
import java.util.concurrent.RecursiveTask;



class CalculateMirror extends RecursiveTask<RGB[]> 
{

	protected RGB[] originalPic;
	protected RGB[] reverse;
	protected int picWid;
	protected int picHite;
	protected int low;
	protected int hi;

    public int SEQUENTIAL_CUTOFF = 10000;

 	public static int findCol(int width,int pixIndex)
	{
		if(width>pixIndex)
		{
			return pixIndex;
		}
		else
		{
			return findCol(width,pixIndex-width);
		}
	}

	//this func wont really return the ROW number 
	//but rather how many indexes have we seen so far to
	//get to the start of the row where our pixel in question is
	public static int findRow(int width,int pixIndex)
	{
		int answer=0;

		while(width<=pixIndex)
		{
			answer+=width;
			pixIndex-=width;
		}
		return answer;
	}


	
    CalculateMirror(int lowArg,int hiArg,int width,RGB[] pixs,RGB[] answerArr) 
    {
    	this.low=lowArg;
    	this.hi=hiArg;
    	this.picWid=width;
    	this.originalPic=pixs;
    	this.reverse=answerArr;
    }

    public RGB [] compute() {
    	//idea if we have under the seq_cutoff elements to deal with
    	//do the sequential implementation so in this case run
    	//thru all pixels, find their appropriate column(within same row)
    	//and place them in that column, essentially flipping the array
   	
    	if((hi-low)<SEQUENTIAL_CUTOFF)	
    	{	
    		for(int index=low;index<hi;index++)
    		{
    		
			int curCol=findCol(picWid,index);
	    	int newCol=(picWid-1)-curCol;
	    	int curRow=findRow(picWid,index);
	    	int newInd=curRow+newCol;

			int red=originalPic[index].R;
			int blue=originalPic[index].G;
			int green=originalPic[index].B;

			RGB addMe = new RGB(red,blue,green);
	    	reverse[newInd]= addMe;
    		}

    	}
    	else
    	{
    		int mid=(low+hi)/2;
    		CalculateMirror calc1=new CalculateMirror(low,mid,picWid,originalPic,reverse);
    		CalculateMirror calc2=new CalculateMirror(mid,hi,picWid,originalPic,reverse);
    		calc1.fork();
    		calc2.compute();
    		calc1.join();

    	}

    	return this.reverse;


    }
    
}


class CalculateGauss extends RecursiveTask<RGB[]>{


	protected RGB[] originalPic;
	protected RGB[] gaussPic;
	protected int picWid;
	protected int picHite;
	protected int low;
	protected int hi;
	protected double gFilter[][];
	protected double sigma;
	protected int radius;

	public static int findCol(int width,int pixIndex)
	{

		if(width>pixIndex)
		{
			return pixIndex;
		}
		else
		{
			return findCol(width,pixIndex-width);
		}
	}

	public static int findRow(int width,int pixIndex)
	{
		int answer=0;

		while(width<=pixIndex)
		{
			answer+=width;
			pixIndex-=width;
		}
		return answer/width;
	}



public int SEQUENTIAL_CUTOFF = Integer.MAX_VALUE;


CalculateGauss(int low,int hi,int picHite,int picWid,RGB[] originalPic,RGB[] gaussPic,double sigma,int radius)
{
	this.low=low;
	this.hi=hi;
	this.picHite=picHite;
	this.picWid=picWid;
	this.originalPic=originalPic;
	this.gaussPic=gaussPic;
	this.radius=radius;
	this.sigma=sigma;
	this.gFilter=Gaussian.gaussianFilter(radius, sigma);
}

public RGB [] compute()
{

	if((hi - low) < SEQUENTIAL_CUTOFF)
	{
		for (int index = low; index < hi; index++)
		{
		double newR = 0.0;
        double newG = 0.0;
        double newB = 0.0;
        int pixCol=findCol(picWid,index);
        int pixRow=findRow(picWid,index);

         for(int curRow=-radius;curRow<radius+1;curRow++)
            {
               int adjustRow = Math.min(Math.max(pixRow+curRow,0),picHite-1);
                for(int curCol=-radius;curCol<radius+1;curCol++)
                {
                   int adjustCol = Math.min(Math.max(pixCol+curCol,0),picWid-1);

                    newR += originalPic[adjustRow*picWid+adjustCol].R * gFilter[curRow+radius][curCol+radius];
                    newG += originalPic[adjustRow*picWid+adjustCol].G * gFilter[curRow+radius][curCol+radius];
                    newB += originalPic[adjustRow*picWid+adjustCol].B * gFilter[curRow+radius][curCol+radius];
             	}
             	
             }
            int addR=(int)Math.round(newR);
            int addG=(int)Math.round(newG);
            int addB=(int)Math.round(newB);
            RGB addMe=new RGB(addR,addG,addB);
            gaussPic[pixRow*picWid+pixCol] = addMe;
        }
	}
	else
	{
 		int mid=(low+hi)/2;
    	CalculateGauss calc1=new CalculateGauss(low,mid,picHite,picWid,originalPic,gaussPic,sigma,radius);
    	CalculateGauss calc2=new CalculateGauss(mid,hi,picHite,picWid,originalPic,gaussPic,sigma,radius);
    	calc1.fork();
    	calc2.compute();
    	calc1.join();
	}
	return this.gaussPic;
}

}


// a marker for code that you need to implement
class ImplementMe extends RuntimeException {}

// an RGB triple
class RGB {
    public int R, G, B;

    RGB(int r, int g, int b) {
	R = r;
	G = g;
	B = b;
    }

    public String toString() { return "(" + R + "," + G + "," + B + ")"; }

}

// code for creating a Gaussian filter
class Gaussian {

    protected static double gaussian(int x, int mu, double sigma) {
	return Math.exp( -(Math.pow((x-mu)/sigma,2.0))/2.0 );
    }

    public static double[][] gaussianFilter(int radius, double sigma) {
	int length = 2 * radius + 1;
	double[] hkernel = new double[length];
	for(int i=0; i < length; i++)
	    hkernel[i] = gaussian(i, radius, sigma);
	double[][] kernel2d = new double[length][length];
	double kernelsum = 0.0;
	for(int i=0; i < length; i++) {
	    for(int j=0; j < length; j++) {
		double elem = hkernel[i] * hkernel[j];
		kernelsum += elem;
		kernel2d[i][j] = elem;
	    }
	}
	for(int i=0; i < length; i++) {
	    for(int j=0; j < length; j++)
		kernel2d[i][j] /= kernelsum;
	}
	return kernel2d;
    }
}

// an object representing a single PPM image
class PPMImage {
    protected int width, height, maxColorVal;
    protected RGB[] pixels;

    PPMImage(int w, int h, int m, RGB[] p) {
	width = w;
	height = h;
	maxColorVal = m;
	pixels = p;
    }

	// parse a PPM file to produce a PPMImage
    public static PPMImage fromFile(String fname) throws FileNotFoundException, IOException {
	FileInputStream is = new FileInputStream(fname);
	BufferedReader br = new BufferedReader(new InputStreamReader(is));
	br.readLine(); // read the P6
	String[] dims = br.readLine().split(" "); // read width and height
	int width = Integer.parseInt(dims[0]);
	int height = Integer.parseInt(dims[1]);
	int max = Integer.parseInt(br.readLine()); // read max color value
	br.close();

	is = new FileInputStream(fname);
	    // skip the first three lines
	int newlines = 0;
	while (newlines < 3) {
	    int b = is.read();
	    if (b == 10)
		newlines++;
	}

	int MASK = 0xff;
	int numpixels = width * height;
	byte[] bytes = new byte[numpixels * 3];
        is.read(bytes);
	RGB[] pixels = new RGB[numpixels];
	for (int i = 0; i < numpixels; i++) {
	    int offset = i * 3;
	    pixels[i] = new RGB(bytes[offset] & MASK, bytes[offset+1] & MASK, bytes[offset+2] & MASK);
	}

	return new PPMImage(width, height, max, pixels);
    }

	// write a PPMImage object to a file
    public void toFile(String fname) throws IOException {
	FileOutputStream os = new FileOutputStream(fname);

	String header = "P6\n" + width + " " + height + "\n" + maxColorVal + "\n";
	os.write(header.getBytes());

	int numpixels = width * height;
	byte[] bytes = new byte[numpixels * 3];
	int i = 0;
	for (RGB rgb : pixels) {
	    bytes[i] = (byte) rgb.R;
	    bytes[i+1] = (byte) rgb.G;
	    bytes[i+2] = (byte) rgb.B;
	    i += 3;
	}
	os.write(bytes);
    }


	// implement using Java 8 Streams
    public PPMImage negate() {
	
		RGB[] newArrayPix=new RGB[this.width*this.height];
		//height*width is the amount of pixels
		newArrayPix = (RGB [])Arrays.stream(pixels).
					parallel().
					map((s)->{
						s.R=maxColorVal-s.R;
						s.G=maxColorVal-s.G;
						s.B=maxColorVal-s.B;
						return s;
					}).toArray(RGB[]::new);
		return new PPMImage(this.width,this.height,this.maxColorVal,newArrayPix);

    }
    

	// implement using Java's Fork/Join library
    public PPMImage mirrorImage() {
	
	RGB [] answers=new RGB[pixels.length];
	//lowArg should be 0 hiArg should be ALL PIXS so hite*wid
   	CalculateMirror mir = new CalculateMirror(0,pixels.length,this.width,this.pixels,answers);
   	answers=mir.compute();
   	return new PPMImage(this.width,this.height,this.maxColorVal,answers);

    }

	// implement using Java's Fork/Join library
    public PPMImage gaussianBlur(int radius, double sigma) {
	RGB [] answers=new RGB[pixels.length];
	CalculateGauss gas= new CalculateGauss(0,pixels.length,this.height,this.width,this.pixels,answers,sigma,radius);
	answers=gas.compute();
	return new PPMImage(this.width,this.height,this.maxColorVal,answers);
    }

    public static int findCol(int width,int pixIndex)
	{

		if(width>pixIndex)
		{
			return pixIndex;
		}
		else
		{
			return findCol(width,pixIndex-width);
		}
	}

	public static int findRow(int width,int pixIndex)
	{
		int answer=0;

		while(width<=pixIndex)
		{
			answer+=width;
			pixIndex-=width;
		}
		return answer/width;
	}


   	public void CalcNewPix(double sigma,int radius, int index, RGB[] blur, double[][] gFilter)
   	{
		double newR = 0.0;
        double newG = 0.0;
        double newB = 0.0;
        int pixCol=findCol(this.width,index);
        int pixRow=findRow(this.width,index);
        for(int curRow=-radius;curRow<radius+1;curRow++)
            {
               int adjustRow = Math.min(Math.max(pixRow+curRow,0),this.height-1);
                for(int curCol=-radius;curCol<radius+1;curCol++)
                {
                   int adjustCol = Math.min(Math.max(pixCol+curCol,0),this.width-1);

                    newR += this.pixels[adjustRow*this.width+adjustCol].R * gFilter[curRow+radius][curCol+radius];
                    newG += this.pixels[adjustRow*this.width+adjustCol].G * gFilter[curRow+radius][curCol+radius];
                    newB += this.pixels[adjustRow*this.width+adjustCol].B * gFilter[curRow+radius][curCol+radius];
             	}
             	
             }
            int addR=(int)Math.round(newR);
            int addG=(int)Math.round(newG);
            int addB=(int)Math.round(newB);
            RGB returner=new RGB(addR,addG,addB);
           	blur[index]=returner;
      }
  


	// implement using Java 8 Streams
    public PPMImage gaussianBlur2(int radius, double sigma) {
	
    	//helper is called CalcNewPix(sigma,radius,index,answers)
    	RGB[] newArrayPix=new RGB[this.pixels.length];
    	double [][] gFilter = Gaussian.gaussianFilter(radius, sigma);

		IntStream.range(0,pixels.length).
		parallel().forEach(i->CalcNewPix(sigma,radius,i,newArrayPix,gFilter));
		

		return new PPMImage(this.width,this.height,this.maxColorVal,newArrayPix);

    }
}




class test {

	public static void main(String [] args)
	{
		RGB[] rgb= new RGB[1];
		rgb[0] = new RGB(0,0,0);
		PPMImage im = new PPMImage(0,0,0,rgb);
		try
		{
			im = im.fromFile("florence.ppm");
			// im=im.mirrorImage();
			im = im.gaussianBlur(60,2.0);
			//im= im.negate();
			im.toFile("timeBlurSeq.jpg");
		}
		catch(IOException e)
		{
			System.out.println("fasd");
		}
	
	}


}