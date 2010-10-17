require File.join(File.dirname(__FILE__), 'spec_helper')

describe "padding" do
  it "should pad the top and bottom of a square image rendered in portrait" do
    image = @resizer.resize(
            :padding_color => 'ffffff',
            :filename => 'black_square.jpg',
            :width => 10,
            :height => 20
    )

    unique_color(image, 0, 0, 10, 5).should == "white"
    unique_color(image, 0, 15, 10, 5).should == "white"
    unique_color(image, 0, 5, 10, 10).should == "black"
  end

  it "should pad the left and right of a square image rendered in landscape" do
    image = @resizer.resize(
            :padding_color => 'ffff00',
            :filename => 'black_square.png',
            :width => 20,
            :height => 10
    )

    unique_color(image, 0, 0, 5, 10).should == "yellow"
    unique_color(image, 15, 0, 5, 10).should == "yellow"
    unique_color(image, 5, 0, 10, 10).should == "black"    
  end
end

def unique_color image, x, y, width, height
  rectangle = image.magick.crop(x, y, width, height)
  rectangle.pixel_color(2,2).to_color
end