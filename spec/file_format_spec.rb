require File.join(File.dirname(__FILE__), 'spec_helper')

describe "file formatting" do
  %w{jpeg gif png}.each do |format|
    it "should be able to resize a .#{format} image" do
          image = @resizer.resize(
            :padding_color => 'ffffff',
            :filename => "black_square.#{format}",
            :width => 30,
            :height => 60
    )

    image.magick.columns.should == 30
    image.magick.rows.should == 60
    image.mime_type.should == "image/#{format}"
    end
  end
end