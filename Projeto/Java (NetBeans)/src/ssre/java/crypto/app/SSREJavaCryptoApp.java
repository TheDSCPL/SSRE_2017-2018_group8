package ssre.java.crypto.app;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.math.BigInteger;
import java.security.InvalidKeyException;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.Security;
import java.security.interfaces.RSAPublicKey;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.RSAPublicKeySpec;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import org.bouncycastle.crypto.BlockCipher;
import org.bouncycastle.crypto.BufferedBlockCipher;
import org.bouncycastle.crypto.CipherParameters;
import org.bouncycastle.crypto.DataLengthException;
import org.bouncycastle.crypto.Digest;
import org.bouncycastle.crypto.InvalidCipherTextException;
import org.bouncycastle.crypto.digests.MD5Digest;
import org.bouncycastle.crypto.digests.SHA1Digest;
import org.bouncycastle.crypto.digests.SHA256Digest;
import org.bouncycastle.crypto.engines.AESEngine;
import org.bouncycastle.crypto.engines.BlowfishEngine;
import org.bouncycastle.crypto.engines.DESedeEngine;
import org.bouncycastle.crypto.modes.CBCBlockCipher;
import org.bouncycastle.crypto.paddings.PaddedBufferedBlockCipher;
import org.bouncycastle.crypto.params.KeyParameter;
import org.bouncycastle.crypto.params.ParametersWithIV;
import org.bouncycastle.jce.provider.BouncyCastleProvider;

// Docs: https://www.bouncycastle.org/docs/docs1.5on/index.html

public class SSREJavaCryptoApp {

    public static final String dataFile = "../data";
    public static RandomAccessFile dataFileStream;
    
    public static final String RSAKeyModulus = "00dd3b4da85453efe54bcea931c356e61ea4f2bbe9f5f363a8803c74c6f862310d9e8926b4744f5cf0efc5ad3a2bc4455e0cd8febc09ab9928279f768381142ec8b55033be4aa998512bf1c67ec6625a402a6b5b92900d1ca0c01de485436862d48e1f74a81bf72cf5e1ce46f041237e7b0c81e87a0d9f65711dfed3219a260a213a6da771a4885063ed26335a0e7d3b22415cd0dd4e3c4c641f856301a32189a5bba26a2f3a9f6ae0e293a1c2dc52d4125506ea26c2eedb5fcce48fc862b26840f40d384aeedfa5f8240e47bde4f02f93c0547cf0ee0d829ac92a827f3a5852a83d56b9c017e0f54955293433aecf4b3bf4e0f24c05aa06c2f499c1eb129267a70adbe351f947d46bcb5c541c608defabfa9152eddf2f30906d72938a063e3a0613b8c5b82e7c0cd6da71dcca78dde25cfab6cd9973b1207fa9a49564e580f7cd5cc672cfaf6aba50564633851ea47df824fc09d9fb8d4490dda39492caa6b8ec0d2a7b5dc30e508b03dd57a9144d1f2d34f76eecb4f47ad9157e0fec183ed611bdb3b771e7be484bac27139706483ee01c0c2a18e50632759192b491532b41227718627718d95e6b845c2c7c6118118d4d355ae121b26e329860baabf509eded99d57c54e9330fcd66fb92792132624dd3db5cfbf1a8bfa291388f12e1f77a114df84f4b46d8a6415f5775c634d6fdb7b484c2623cec31551a53bb23758d1d2b";
    public static final String RSAKeyE = "10001";
    public static final String blockCipherKey = "ligTNvexz0Zr1cJQEBrEbRwyHkTYJORV";
    public static final String blockCipherIV = "9ycqc1GAguyrdHVXdXQBGbA4MSTD44G3";
    
    public static void main(String[] args) throws FileNotFoundException, IOException {
        Security.addProvider(new BouncyCastleProvider());
        
        int iterCount = Integer.parseInt(args[0]);
        int algorithm = Integer.parseInt(args[1]);
        //int library = Integer.parseInt(args[2]);
        int mode = Integer.parseInt(args[3]);
        
        dataFileStream = new RandomAccessFile(dataFile, "r");
        
        boolean encrypt = mode == 0;
        for (int i = 0; i < iterCount; i++) {
            dataFileStream.seek(0);
            
            switch (algorithm) {
                case 0: hash(new MD5Digest()); break;
                case 1: hash(new SHA1Digest()); break;
                case 2: hash(new SHA256Digest()); break;
                case 3: blockCipher(new DESedeEngine(), 24, encrypt); break;
                case 4: blockCipher(new AESEngine(), 32, encrypt); break;
                case 5: blockCipher(new BlowfishEngine(), 16, encrypt); break;
                case 6: asymmetricBlockCipher(); break;
            }
        }
    }
    
    public static void hash(Digest digester) {
        int inputBlockSize = 512;
        int outputBlockSize = digester.getDigestSize();
        byte[] inputBuffer = new byte[inputBlockSize];        
        byte[] outputBuffer = new byte[outputBlockSize];
        
        try {
            int readBytes;
            while ((readBytes = dataFileStream.read(inputBuffer)) != -1) {
                digester.update(inputBuffer, 0, readBytes);
            }
            
            digester.doFinal(outputBuffer, 0);    
        }
        catch (DataLengthException | IllegalStateException | IOException ex) {
            Logger.getLogger(SSREJavaCryptoApp.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    public static void blockCipher(BlockCipher engine, int keySize, boolean forEncryption) {
	BufferedBlockCipher cipher = new PaddedBufferedBlockCipher(new CBCBlockCipher(engine));
        
        int inputBlockSize = cipher.getBlockSize();
        
        CipherParameters params = new ParametersWithIV(
                new KeyParameter(blockCipherKey.substring(0, keySize).getBytes()),
                blockCipherIV.substring(0, inputBlockSize).getBytes()
        );
        
        int outputBlockSize = cipher.getOutputSize(inputBlockSize) * 3;
        byte[] inputBuffer = new byte[inputBlockSize];
        byte[] outputBuffer = new byte[outputBlockSize];
        
        try {
            cipher.init(forEncryption, params);
            
            int readBytes;
            while ((readBytes = dataFileStream.read(inputBuffer)) != -1) {
                cipher.processBytes(inputBuffer, 0, readBytes, outputBuffer, 0);
                print(outputBuffer);
            }
            
            cipher.doFinal(outputBuffer, 0);
        }
        catch (DataLengthException | IllegalStateException | InvalidCipherTextException | IOException ex) {
            Logger.getLogger(SSREJavaCryptoApp.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    public static Cipher RSAcipher;
    public static KeyFactory keyFactory;
    public static RSAPublicKeySpec pubKeySpec;
    public static RSAPublicKey pubKey;
    public static boolean loadedRSA = false;

    public static void asymmetricBlockCipher() {
        try {
            if (!loadedRSA) {
                RSAcipher = Cipher.getInstance("RSA/NONE/OAEPWithSHA1AndMGF1Padding", "BC");
                keyFactory = KeyFactory.getInstance("RSA", "BC");
                pubKeySpec = new RSAPublicKeySpec(new BigInteger(RSAKeyModulus, 16), new BigInteger(RSAKeyE, 16));
                pubKey = (RSAPublicKey) keyFactory.generatePublic(pubKeySpec);
                loadedRSA = true;
            }
            
            byte[] input = new byte[(int) dataFileStream.length()];
            dataFileStream.read(input);
            
            RSAcipher.init(Cipher.ENCRYPT_MODE, pubKey);
            print(RSAcipher.doFinal(input));
        }
        catch (InvalidKeyException | NoSuchAlgorithmException | NoSuchProviderException | NoSuchPaddingException | InvalidKeySpecException | IllegalBlockSizeException | BadPaddingException | IOException ex) {
            Logger.getLogger(SSREJavaCryptoApp.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    
    private static final String digits = "0123456789abcdef";
    public static String toHex(byte[] data, int length) {
        StringBuilder buf = new StringBuilder();
        
        for (int i = 0; i != length; i++) {
            int	v = data[i] & 0xff;
            buf.append(digits.charAt(v >> 4));
            buf.append(digits.charAt(v & 0xf));
        }
        
        return buf.toString();
    }
    public static void print(byte[] data) {
        System.out.printf("%s\n", toHex(data, data.length));
    }
    
}
