require File.join(File.dirname(__FILE__), 'spec_helper')

describe 'resizing' do
  it "should resize my an image to a portrait" do
    image = @resizer.resize(
            :padding_color => 'ffffff',
            :filename => 'cat-in-hat.jpg',
            :width => 30,
            :height => 60
    )

    image.mime_type.should == "image/jpeg"
    image.magick.columns.should == 30
    image.magick.rows.should == 60
  end

  it "should resize my an image to a landscape" do
    image = @resizer.resize(
            :filename => 'cat-in-hat.jpg',
            :padding_color => '000000',
            :width => 600,
            :height => 200
    )

    image.magick.columns.should == 600
    image.magick.rows.should == 200
  end
end
