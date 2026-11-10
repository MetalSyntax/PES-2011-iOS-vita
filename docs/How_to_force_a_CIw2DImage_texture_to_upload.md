# How to force a CIw2DImage texture to upload

Original URL: http://www.drmop.com/index.php/2011/10/19/marmalade-sdk-bitesize-tutorial-how-to-force-a-ciw2dimage-texture-to-upload/

Welcome to another bite size Marmalade SDK tutorial. I’ve seen this question pop up a few times in the Marmalade SDk forums so I thought that I would cover it here as a bite size tutorial.

You are using the Iw2D module to render handle 2D images, but you need to force the images to be uploaded to texture RAM before the game begins. You took a quick look at CIw2DImage to discover that it is in fact an abstract class with not a lot of info in there, except a few pure virtual methods  for retrieving width, height and the material associated with the CIw2DImage.

You can call GetMaterial() to return the material that makes up your CIw2DImage() and then in turn, call GetTexture() on the returned material to locate its texture. Finally you can call Upload() on the texture to tell Marmalade to upload it. Here’s a quick example code to show what I mean:

CIwMaterial\* mat = your\_iw2d\_image->GetMaterial();  
CIwTexture\* texture = mat->GetTexture();  
texture->Upload();

Don’t forget that unless you mark the texture as modifiable using texture->SetModifiable(true), the system will delete your pixel / palette data to free up memory.