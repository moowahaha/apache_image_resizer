require File.join(File.dirname(__FILE__), 'spec_helper')

describe "cgi parameters" do
  it "should throw an error if there are no dimensions specified" do
    lambda {
      @resizer.resize(
              :padding_color => 'ffffff',
              :filename => 'cat-in-hat.jpg',
              :width => '',
              :height => ''
      )
    }.should raise_error(/Status: 400 Missing parameters: width height/)
  end

  it "should throw an error if width or height are 0" do
    lambda {
      @resizer.resize(
              :padding_color => 'ffffff',
              :filename => 'cat-in-hat.jpg',
              :width => 0,
              :height => 0
      )
    }.should raise_error(/Status: 400 Missing parameters: width height/)
  end
end