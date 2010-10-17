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
end