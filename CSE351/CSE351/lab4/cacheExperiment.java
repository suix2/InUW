
class cacheExperiment {

	public static void main(String[] arg) {
		int src[][] = new int[4096][4096];
		int dst[][] = new int[4096][4096];
		
		int rep;
		int i, j;
		
		for ( rep = 0; rep < 10; rep++ ) {
			for ( i = 0; i < 4096; i++) {
				for ( j = 0; j < 4096; j++) {
					src[j][i] = i * rep;
					dst[j][i] = src[j][i];
				}
			}
		}
	}

}
